#include "pch.h"
#include "StringsView.h"
#include "FormatHelper.h"
#include "SortHelper.h"
#include <algorithm>

StringsView::StringsView(DataTarget* dt) : _target(dt) {
}

int StringsView::GetItemCount() {
	CWaitCursor wait;
	_strings.clear();
	_strings.reserve(1 << 15);
	_target->EnumObjects([](auto& obj, auto param) {
		if (obj.ObjectType == OBJ_STRING) {
			auto p = static_cast<StringsView*>(param);
			p->GetStringValue(obj);
			p->_strings.push_back(std::move(obj));
		}
		return true;
		}, this);

	_allStrings = _strings;

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
	auto& str = _strings[row];

	switch (col) {
		case 0: return FormatHelper::ToHex(str.Address);
		case 1:	return FormatHelper::ToDec((str.Size - 3 * sizeof(PVOID)) / sizeof(WCHAR) - 1);
		case 2: return str.StringValue;
	}

	return L"";
}

bool StringsView::Sort(int column, bool ascending) {
	std::sort(_strings.begin(), _strings.end(), [=](const auto& str1, const auto& str2) {
		return CompareItems(str1, str2, column, ascending);
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

void StringsView::ApplyTextFilter(PCWSTR filter) {
	CString sfilter(filter);
	if (sfilter.IsEmpty()) {
		_strings = _allStrings;
	}
	else {
		_strings.clear();
		CWaitCursor wait;
		sfilter.MakeUpper();
		for (auto& str : _allStrings) {
			auto temp(str.StringValue);
			temp.MakeUpper();
			if (temp.Find(sfilter) >= 0)
				_strings.push_back(str);
		}
	}
	_gv->SetListViewItemCount(static_cast<int>(_strings.size()));
}

LRESULT StringsView::CDialogBar::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&) {
	return LRESULT();
}

LRESULT StringsView::CDialogBar::OnColorDialog(UINT, WPARAM, LPARAM, BOOL&) {
	return (LRESULT)::GetSysColorBrush(COLOR_WINDOW);
}

LRESULT StringsView::CDialogBar::OnTimer(UINT, WPARAM id, LPARAM, BOOL&) {
	if (id == 100) {
		KillTimer(100);
		CString text;
		GetDlgItemText(IDC_TEXT, text);
		m_View.ApplyTextFilter(text);
	}
	return 0;
}

LRESULT StringsView::CDialogBar::OnTextChanged(WORD, WORD, HWND, BOOL&) {
	SetTimer(100, 500, nullptr);

	return 0;
}
