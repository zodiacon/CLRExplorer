#pragma once

#include "Interfaces.h"
#include "DataTarget.h"

class ObjectsView : public IGenericListViewCallback {
public:
	ObjectsView(DataTarget* dt);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;

private:
	DataTarget* _target;
	std::vector<ObjectInfo> _objects;
};

