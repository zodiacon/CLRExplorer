#pragma once

#include "Interfaces.h"
#include "DataTarget.h"
#include "SortedFilteredVector.h"
#include "QuickFIlterDialogBar.h"

class AssembliesView : public IGenericListViewCallback, public IDialogBarProvider {
public:
	AssembliesView(DataTarget* dt);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;

	// IDialogBarProvider
	HWND Create(HWND hParent) override;

	void SetFilter(PCWSTR text);

private:
	class CDialogBar : public CQuickFilterDialogBar<CDialogBar> {
	public:
		CDialogBar(AssembliesView& view);

		void ApplyTextFilter(PCWSTR text);

	private:
		AssembliesView& m_View;
	};

private:
	DataTarget* _target;
	SortedFilteredVector<AssemblyInfo> _items;
	IGenericView* _gv;
};

