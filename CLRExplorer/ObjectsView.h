#pragma once

#include "Interfaces.h"
#include "DataTarget.h"

class ObjectsView : public IGenericListViewCallback {
public:
	ObjectsView(DataTarget* dt, const MethodTableInfo& mt);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericListView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;

private:
	DataTarget* _target;
	MethodTableInfo _mt;
	std::vector<ObjectInfo> _objects;
};

