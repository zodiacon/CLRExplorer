#include "pch.h"
#include "SummaryView.h"
#include "DataTarget.h"
#include <string>

SummaryView::SummaryView(DataTarget* target) : _target(target) {
}

int SummaryView::GetItemCount() {
	return 0;
}

bool SummaryView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 150);
	lv.InsertColumn(1, L"Value", LVCFMT_LEFT, 250);

	return true;
}

CString SummaryView::GetItemText(int row, int col) {
	if (col == 0) {
		static PCWSTR names[] = {
			L"AppDomains", L"Assemblies", L"Total Threads", L"Managed Threads",
		};
		return names[row];
	}

	switch (row) {
		case 0: return std::to_wstring(_target->EnumAppDomains().size()).c_str();
		case 1: return std::to_wstring(_target->EnumAssemblies().size()).c_str();
	}

	return CString();
}

bool SummaryView::Sort(int column, bool ascending) {
	return false;
}
