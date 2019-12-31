#include "pch.h"
#include "ThreadsView.h"
#include "resource.h"
#include "FormatHelper.h"
#include <algorithm>
#include "SortHelper.h"

ThreadsView::ThreadsView(DataTarget* dt) : _target(dt) {
}

int ThreadsView::GetItemCount() {
	_threads = _target->EnumThreads(_showDeadObjects);
	return static_cast<int>(_threads.size());
}

bool ThreadsView::Init(CListViewCtrl& lv, IGenericListView* glv) {
	_glv = glv;

	lv.InsertColumn(0, L"Managed ID", LVCFMT_CENTER, 80);
	lv.InsertColumn(1, L"OS ID", LVCFMT_RIGHT, 120);
	lv.InsertColumn(2, L"State", LVCFMT_RIGHT, 90);
	lv.InsertColumn(3, L"Locks", LVCFMT_CENTER, 70);
	lv.InsertColumn(4, L"TEB", LVCFMT_RIGHT, 140);
	lv.InsertColumn(5, L"Details", LVCFMT_LEFT, 450);

	CImageList images;
	images.Create(16, 16, ILC_COLOR32, 2, 2);
	images.AddIcon(AtlLoadIcon(IDI_THREAD));
	images.AddIcon(AtlLoadIcon(IDI_THREAD_DEAD));
	lv.SetImageList(images, LVSIL_SMALL);

	return true;
}

CString ThreadsView::GetItemText(int row, int col) {
	auto& data = _threads[row];

	switch (col) {
		case 0: return FormatHelper::ToDec(data.corThreadId, 4);
		case 1: return data.osThreadId == 0 ? L"" : FormatHelper::ToDec(data.osThreadId) + L" (" + FormatHelper::ToHex(data.osThreadId) + L")";
		case 2: return FormatHelper::ToHex(data.state);
		case 3: return FormatHelper::ToDec(data.lockCount);
		case 4: return FormatHelper::ToHex((PVOID)data.teb);
		case 5: return ThreadStateToString(data.state);
	}

	return L"";
}

bool ThreadsView::Sort(int column, bool ascending) {
	std::sort(_threads.begin(), _threads.end(), [=](auto& t1, auto& t2) {
		return CompareItems(t1, t2, column, ascending);
		});

	return true;
}

int ThreadsView::GetIcon(int row) {
	return _threads[row].osThreadId > 0 ? 0 : 1;
}

bool ThreadsView::Init(CToolBarCtrl& tb) {
	CImageList tbImages;
	tbImages.Create(24, 24, ILC_COLOR32, 8, 4);
	tb.SetImageList(tbImages);

	struct {
		UINT id;
		int image;
		int style = BTNS_BUTTON;
	} buttons[] = {
		{ IdShowDeadThreads, IDI_THREAD_DEAD, BTNS_CHECK },
		{ IdShowNativeThreads, IDI_THREAD_NATIVE, BTNS_CHECK },
	};
	for (auto& b : buttons) {
		if (b.id == 0)
			tb.AddSeparator(0);
		else {
			int image = tbImages.AddIcon(AtlLoadIconImage(b.image, 0, 24, 24));
			tb.AddButton(b.id, b.style, TBSTATE_ENABLED, image, nullptr, 0);
		}
	}
	return true;
}

bool ThreadsView::OnCommand(WORD id) {
	switch (id) {
		case IdShowDeadThreads:
			_showDeadObjects = !_showDeadObjects;
			_glv->Refresh();
			return true;
	}
	return false;
}

CString ThreadsView::ThreadStateToString(DWORD state) {
	static const struct {
		DWORD state;
		PCWSTR text;
	} states[] = {
		{0x1, L"Abort Requested"},
		{0x2, L"GC Suspend Pending"},
		{0x4, L"User Suspend Pending"},
		{0x8, L"Debug Suspend Pending"},
		{0x10, L"GC On Transitions"},
		{0x20, L"Legal to Join"},
		{0x40, L"Yield Requested"},
		{0x80, L"Hijacked by GC"},
		{0x100, L"Blocking GC for Stack Overflow"},
		{0x200, L"Background"},
		{0x400, L"Unstarted"},
		{0x800, L"Dead"},
		{0x1000, L"CLR Owns"},
		{0x2000, L"CoInitialized"},
		{0x4000, L"STA"},
		{0x8000, L"MTA"},
		{0x10000, L"Reported Dead"},
		{0x20000, L"Fully initialized"},
		{0x40000, L"Task Reset"},
		{0x80000, L"Sync Suspended"},
		{0x100000, L"Debug Will Sync"},
		{0x200000, L"Stack Crawl Needed"},
		{0x400000, L"Suspend Unstarted"},
		{0x800000, L"Aborted"},
		{0x1000000, L"Thread Pool Worker"},
		{0x2000000, L"Interruptible"},
		{0x4000000, L"Interrupted"},
		{0x8000000, L"Completion Port"},
		{0x10000000, L"Abort Initiated"},
		{0x20000000, L"Finalized"},
		{0x40000000, L"Failed to Start"},
		{0x80000000, L"Detached"},
	};

	CString text;
	for (auto& item : states)
		if (item.state & state)
			text += item.text + CString(L", ");
	if (!text.IsEmpty())
		text = text.Left(text.GetLength() - 2);
	return text;
}

bool ThreadsView::CompareItems(const ThreadInfo& t1, const ThreadInfo& t2, int col, bool asc) {
	switch (col) {
		case 0: return SortHelper::SortNumbers(t1.corThreadId, t2.corThreadId, asc);
		case 1: return SortHelper::SortNumbers(t1.osThreadId, t2.osThreadId, asc);
		case 2: case 5:
			return SortHelper::SortNumbers(t1.state, t2.state, asc);
		case 3: return SortHelper::SortNumbers(t1.lockCount, t2.lockCount, asc);
		case 4: return SortHelper::SortNumbers(t1.teb, t2.teb, asc);

	}
	ATLASSERT(false);
	return false;
}
