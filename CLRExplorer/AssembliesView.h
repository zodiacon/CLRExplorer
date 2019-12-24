#pragma once

#include "Interfaces.h"
#include "DataTarget.h"

class AssembliesView : public IGenericListViewCallback {
public:
	AssembliesView(DataTarget* dt);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;

	static bool CompareItems(const AssemblyInfo& a1, const AssemblyInfo& a2, int col, bool asc);

private:
	DataTarget* _target;
	std::vector<AssemblyInfo> _items;
};

