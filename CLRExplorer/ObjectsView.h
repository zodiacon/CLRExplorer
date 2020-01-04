#pragma once

#include "Interfaces.h"
#include "DataTarget.h"

class ObjectsView : public IGenericListViewCallback {
public:
	ObjectsView(DataTarget* dt, CLRDATA_ADDRESS mt);

	const MethodTableInfo& GetMethodTable() const;

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericListView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	
private:
	static bool CompareItems(const ObjectInfo& obj1, const ObjectInfo& obj2, int col, bool asc);

private:
	DataTarget* _target;
	MethodTableInfo _mt;
	std::vector<ObjectInfo> _objects;
	CLRDATA_ADDRESS _address;
};

