#pragma once

#include "Interfaces.h"
#include "DataTarget.h"

class SyncBlocksView : public IGenericListViewCallback {
public:
	SyncBlocksView(DataTarget* dt);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericListView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;

private:
	DataTarget* _target;
	std::vector<SyncBlockInfo> _items;
};

