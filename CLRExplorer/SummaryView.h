#pragma once

#include "Interfaces.h"

class DataTarget;

class SummaryView : public IGenericListViewCallback {
public:
	SummaryView(DataTarget* target);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;

private:
	DataTarget* _target;
};

