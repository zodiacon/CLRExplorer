#include "pch.h"
#include "HeapStatsView.h"
#include "FormatHelper.h"
#include "SortHelper.h"
#include "resource.h"

HeapStatsView::HeapStatsView(DataTarget* dt, IMainFrame* frame) :_target(dt), _frame(frame) {
}

int HeapStatsView::GetItemCount() {
	CWaitCursor wait;
	_items.Set(_target->GetHeapStats());

	return static_cast<int>(_items.Size());
}

bool HeapStatsView::Init(CListViewCtrl& lv, IGenericListView* glv) {
	lv.InsertColumn(0, L"Type Name", LVCFMT_LEFT, 450);
	lv.InsertColumn(1, L"Method Table", LVCFMT_RIGHT, 120);
	lv.InsertColumn(2, L"Object Count", LVCFMT_RIGHT, 100);
	lv.InsertColumn(3, L"Total Size", LVCFMT_RIGHT, 120);

	CImageList images;
	images.Create(16, 16, ILC_COLOR32, 5, 3);
	lv.SetImageList(images, LVSIL_SMALL);

	UINT ids[] = { IDI_STRING, IDI_FREE, IDI_OBJECT, IDI_GRID, IDI_CLASS };
	for (auto id : ids)
		images.AddIcon(AtlLoadIconImage(id, 0, 16, 16));

	_glv = glv;

	return true;
}

CString HeapStatsView::GetItemText(int row, int col) {
	const auto& item = _items[row];

	switch (col) {
		case 0: return item.TypeName;
		case 1: return FormatHelper::ToHex(item.MethodTable);
		case 2: return FormatHelper::ToDec(item.ObjectCount);
		case 3: return FormatHelper::ToDec(item.TotalSize);
	}
	ATLASSERT(false);
	return L"";
}

bool HeapStatsView::Sort(int column, bool ascending) {
	_items.Sort([=](auto& item1, auto& item2) {
		switch (column) {
			case 0: return SortHelper::SortStrings(item1.TypeName, item2.TypeName, ascending);
			case 1: return SortHelper::SortNumbers(item1.MethodTable, item2.MethodTable, ascending);
			case 2: return SortHelper::SortNumbers(item1.ObjectCount, item2.ObjectCount, ascending);
			case 3: return SortHelper::SortNumbers(item1.TotalSize, item2.TotalSize, ascending);
		}
		return false;
		});
	return true;
}

int HeapStatsView::GetIcon(int row) {
	const auto& item = _items[row];
	return (int)item.Type;
}

void HeapStatsView::OnContextMenu(const POINT& pt, int selected) {
	if (selected < 0)
		return;

	CMenu menu;
	menu.LoadMenuW(IDR_CONTEXT);
	auto cmd = (UINT)_frame->ShowContextMenu(menu.GetSubMenu(0), pt, TPM_RETURNCMD);
	const auto& item = _items[selected];

	switch (cmd) {
		case ID_HEAPSTATS_SHOWALLINSTANCES:
			ShowAllInstances(selected);
			break;
	}
}

void HeapStatsView::OnDoubleClick(int selected) {
	if (selected >= 0)
		ShowAllInstances(selected);
}

HWND HeapStatsView::Create(HWND hParent) {
	auto dlg = new CDialogBar(*this);
	return dlg->Create(hParent);
}

void HeapStatsView::ShowAllInstances(int selected) {
	const auto& item = _items[selected];
	int tab = _frame->FindTab((DWORD_PTR)item.MethodTable);
	if (tab < 0) {
		_frame->AddTab((DWORD_PTR)item.MethodTable, NodeType::MethodTable, NodeType::HeapStats);
	}
}

void HeapStatsView::SetFilter(PCWSTR text) {
	CString stext(text);
	stext.MakeUpper();
	_items.Filter([&](auto& value) {
		CString stype(value.TypeName);
		stype.MakeUpper();
		return stype.Find(stext) >= 0;
		});
	_glv->SetListViewItemCount(static_cast<int>(_items.FilteredSize()));
}
