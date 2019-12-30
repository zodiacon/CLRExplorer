#include "pch.h"
#include "StringsView.h"
#include "FormatHelper.h"
#include "SortHelper.h"
#include <algorithm>

StringsView::StringsView(DataTarget* dt) : _target(dt) {
}

int StringsView::GetItemCount() {
	CWaitCursor wait;
	_allStrings.clear();
	_strings.clear();
	_allStrings.reserve(1 << 15);
	_strings.reserve(1 << 15);
	_target->EnumObjects([](auto& obj, auto param) {
		if (obj.ObjectType == OBJ_STRING) {
			auto p = static_cast<StringsView*>(param);
			p->GetStringValue(obj);
			p->_strings.push_back(static_cast<int>(p->_strings.size()));
			p->_allStrings.push_back(std::move(obj));
		}
		return true;
		}, this);

	return static_cast<int>(_strings.size());
}

bool StringsView::Init(CListViewCtrl& lv, IGenericView* gv) {
	lv.InsertColumn(0, L"Address", LVCFMT_LEFT, 120);
	lv.InsertColumn(1, L"Length", LVCFMT_RIGHT, 80);
	lv.InsertColumn(2, L"Value", LVCFMT_LEFT, 500);
	
	_gv = gv;

	return true;
}

CString StringsView::GetItemText(int row, int col) {
	auto index = _strings[row];
	auto& str = _allStrings[index];

	switch (col) {
		case 0: return FormatHelper::ToHex(str.Address);
		case 1:	return FormatHelper::ToDec((str.Size - 3 * sizeof(PVOID)) / sizeof(WCHAR) - 1);
		case 2: return str.StringValue;
	}

	return L"";
}

bool StringsView::Sort(int column, bool ascending) {
	std::sort(_strings.begin(), _strings.end(), [=](const auto& i1, const auto& i2) {
		bool compare = CompareItems(_allStrings[i1], _allStrings[i2], column, ascending);
		return compare;
		});

	return true;
}

bool StringsView::CompareItems(const ObjectInfo& str1, const ObjectInfo& str2, int col, bool asc) {
	switch (col) {
		case 0:	return SortHelper::SortNumbers(str1.Address, str2.Address, asc);
		case 1: return SortHelper::SortNumbers(str1.Size, str2.Size, asc);
		case 2: return SortHelper::SortStrings(str1.StringValue, str2.StringValue, asc);
	}
	return false;
}

void StringsView::GetStringValue(ObjectInfo& str) {
	str.StringValue = _target->GetObjectString(str.Address);
}

HWND StringsView::Create(HWND hParent) {
	_dialogBar = new CDialogBar(*this);
	return _dialogBar->Create(hParent);
}

void StringsView::SetFilter(PCWSTR filter) {
	CString sfilter(filter);
	if (sfilter.IsEmpty()) {
		int i = 0;
		for (auto& n : _strings)
			n = i;
	}
	else {
		_strings.clear();
		CWaitCursor wait;
		sfilter.MakeUpper();
		int i = 0;
		for (auto& str : _allStrings) {
			auto temp(str.StringValue);
			temp.MakeUpper();
			if (temp.Find(sfilter) >= 0)
				_strings.push_back(i);
			i++;
		}
	}
	_gv->SetListViewItemCount(static_cast<int>(_strings.size()));
}

