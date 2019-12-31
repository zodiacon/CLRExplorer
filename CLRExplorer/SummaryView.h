#pragma once

#include "Interfaces.h"
#include "DataTarget.h"

class SummaryView : public IGenericListViewCallback {
public:
	SummaryView(DataTarget* target);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericListView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	bool CanSort(int col) const override;

private:
	DataTarget* _target;
	DacpThreadStoreData _threadStore;
};

