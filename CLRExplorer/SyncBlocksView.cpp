#include "pch.h"
#include "SyncBlocksView.h"
#include "FormatHelper.h"
#include "resource.h"
#include <algorithm>

SyncBlocksView::SyncBlocksView(DataTarget* dt) : _target(dt) {

}

int SyncBlocksView::GetItemCount() {
	_items = _target->EnumSyncBlocks(false);

	return static_cast<int>(_items.size());
}

bool SyncBlocksView::Init(CListViewCtrl& lv, IGenericView* glv) {
	lv.InsertColumn(0, L"Index", LVCFMT_CENTER, 70);
	lv.InsertColumn(1, L"Object", LVCFMT_RIGHT, 100);
	lv.InsertColumn(2, L"Type", LVCFMT_LEFT, 150);
	lv.InsertColumn(3, L"Sync Block", LVCFMT_RIGHT, 120);
	lv.InsertColumn(4, L"Monitors Held", LVCFMT_CENTER, 90);
	lv.InsertColumn(5, L"Owner Thread", LVCFMT_CENTER, 120);
	lv.InsertColumn(6, L"Owner Thread ID", LVCFMT_LEFT, 150);
	lv.InsertColumn(7, L"Recursion", LVCFMT_CENTER, 80);
	lv.InsertColumn(8, L"COM Flags", LVCFMT_RIGHT, 80);

	CImageList images;
	images.Create(16, 16, ILC_COLOR32, 2, 2);
	images.AddIcon(AtlLoadIcon(IDI_LOCK_OPEN));
	images.AddIcon(AtlLoadIcon(IDI_LOCK_LOCKED));
	lv.SetImageList(images, LVSIL_SMALL);

	return true;
}

CString SyncBlocksView::GetItemText(int row, int col) {
	auto& item = _items[row];
	switch (col) {
		case 0: return FormatHelper::ToDec(item.Index, 4);
		case 1: return FormatHelper::ToHex(item.Object);
		case 2: return _target->GetObjectClassName(item.Object);
		case 3: return FormatHelper::ToHex(item.SyncBlockPointer);
		case 4: return FormatHelper::ToDec(item.MonitorHeld);
		case 5: return item.HoldingThread == 0 ? L"" : FormatHelper::ToHex(item.HoldingThread);
		case 6:
			if (item.HoldingThread) {
				auto td = _target->GetThreadData(item.HoldingThread);
				return L"Managed ID: " + FormatHelper::ToDec(td.corThreadId) + L" OSID: " + FormatHelper::ToDec(td.osThreadId);
			}
			else
				return L"";
		case 7: return item.HoldingThread == 0 ? L"" : FormatHelper::ToDec(item.Recursion);
		case 8: return FormatHelper::ToHex(item.COMFlags);
	}

	return L"";
}

bool SyncBlocksView::Sort(int column, bool ascending) {
	return false;
}

int SyncBlocksView::GetIcon(int row) {
	return _items[row].HoldingThread ? 1 : 0;
}
