#include "pch.h"
#include "SummaryView.h"
#include "DataTarget.h"
#include "FormatHelper.h"

static PCWSTR names[] = {
	L"Image Path", L"Process ID", L"Start Time",
	L"AppDomains", L"Total Assemblies", L"Total Threads", L"Managed Threads (Live)",
	L"GC Type", L"Heap Count",
};

SummaryView::SummaryView(DataTarget* target) : _target(target) {
}

int SummaryView::GetItemCount() {
	_threadStore = _target->GetThreadsStats();
	return _countof(names);
}

bool SummaryView::Init(CListViewCtrl& lv, IGenericListView* glv) {
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
		case 0: return _target->GetProcessPathName();
		case 1: return FormatHelper::ToDec(_target->GetProcessId());
		case 2: return CTime(_target->GetProcessStartTime()).Format(L"%c");
		case 3: return std::to_wstring(_target->GetAppDomainCount()).c_str();
		case 4: return std::to_wstring(_target->EnumAssemblies().size()).c_str();
		case 6:	// managed threads
			text.Format(L"%d (%d)", _threadStore.threadCount, _threadStore.threadCount- _threadStore.deadThreadCount);
			break;

		case 7:	return _target->GetGCInfo().bServerMode ? L"Server" : L"Workstation"; 
		case 8: return FormatHelper::ToDec(_target->GetGCInfo().HeapCount);
	}

	return text;
}

bool SummaryView::Sort(int column, bool ascending) {
	return false;
}

bool SummaryView::CanSort(int col) const {
	return false;
}
