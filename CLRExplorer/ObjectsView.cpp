#include "pch.h"
#include "ObjectsView.h"
#include "FormatHelper.h"
#include <algorithm>
#include "SortHelper.h"

ObjectsView::ObjectsView(DataTarget* dt, CLRDATA_ADDRESS mt) : _target(dt), _address(mt) {
	_mt = dt->GetMethodTableInfo(mt);
}

const MethodTableInfo& ObjectsView::GetMethodTable() const {
	return _mt;
}

int ObjectsView::GetItemCount() {
	_objects.clear();
	_objects.reserve(256);

	_target->EnumObjects([&](auto& obj) {
		if (obj.MethodTable == _address)
			_objects.push_back(obj);
		return true;
		});

	return static_cast<int>(_objects.size());
}

bool ObjectsView::Init(CListViewCtrl& lv, IGenericListView* glv) {
	lv.InsertColumn(0, L"", LVCFMT_LEFT, 1);
	lv.InsertColumn(1, L"Address", LVCFMT_RIGHT, 120);
	lv.InsertColumn(2, L"Size", LVCFMT_RIGHT, 70);

	return true;
}

CString ObjectsView::GetItemText(int row, int col) {
	const auto& item = _objects[row];

	switch (col) {
		case 1: return FormatHelper::ToHex(item.Address);
		case 2: return FormatHelper::ToDec(item.Size);
	}
	return L"";
}

bool ObjectsView::Sort(int column, bool ascending) {
	std::sort(_objects.begin(), _objects.end(), [=](auto& obj1, auto& obj2) {
		return CompareItems(obj1, obj2, column, ascending);
		});

	return true;
}

bool ObjectsView::CompareItems(const ObjectInfo& obj1, const ObjectInfo& obj2, int col, bool asc) {
	switch (col) {
		case 1: return SortHelper::SortNumbers(obj1.Address, obj2.Address, asc);
		case 2: return SortHelper::SortNumbers(obj1.Size, obj2.Size, asc);
	}

	ATLASSERT(false);
	return false;
}
