#include "pch.h"
#include "SummaryView.h"
#include "DataTarget.h"
#include <string>

static PCWSTR names[] = {
	L"AppDomains", L"Total Assemblies", L"Total Threads", L"Managed Threads (Live)",
	L"GC Type", L"Heap Count",
};

SummaryView::SummaryView(DataTarget* target) : _target(target) {
}

int SummaryView::GetItemCount() {
	_threadStore = _target->GetThreadsStats();
	return _countof(names);
}

bool SummaryView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 200);
	lv.InsertColumn(1, L"Value", LVCFMT_LEFT, 300);

	return true;
}

CString SummaryView::GetItemText(int row, int col) {
	if (col == 0) {
		return names[row];
	}

	CString text;
	switch (row) {
		case 0: return std::to_wstring(_target->GetAppDomainCount()).c_str();
		case 1: return std::to_wstring(_target->EnumAssemblies().size()).c_str();
		case 3:	// managed threads
			text.Format(L"%d (%d)", _threadStore.threadCount, _threadStore.threadCount- _threadStore.deadThreadCount);
			break;

		case 4:	
			text = _target->GetGCInfo().bServerMode ? L"Server" : L"Workstation"; 
			break;
		case 5: 
			text.Format(L"%d", _target->GetGCInfo().HeapCount);
			break;

	}

	return text;
}

bool SummaryView::Sort(int column, bool ascending) {
	return false;
}

bool SummaryView::CanSort(int col) const {
	return false;
}
