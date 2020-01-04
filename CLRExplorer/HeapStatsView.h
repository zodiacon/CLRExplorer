#pragma once

#include "Interfaces.h"
#include "DataTarget.h"
#include "SortedFilteredVector.h"
#include "QuickFIlterDialogBar.h"
#include "resource.h"

class HeapStatsView : 
	public IGenericListViewCallback, public IDialogBarProvider {
public:
	HeapStatsView(DataTarget* dt, IMainFrame* frame);

	void SetFilter(PCWSTR text);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericListView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;
	void OnContextMenu(const POINT& pt, int selected) override;
	void OnDoubleClick(int selected) override;

	// IDialogBarProvider
	HWND Create(HWND hParent) override;

private:
	void ShowAllInstances(int selected);

	class CDialogBar : public CQuickFilterDialogBar<CDialogBar> {
	public:
		CDialogBar(HeapStatsView& view) : BaseClass(IDD_STRINGS_DIALOGBAR), m_View(view) {}

		void ApplyTextFilter(PCWSTR text) {
			m_View.SetFilter(text);
		}
		HeapStatsView& m_View;
	};

private:
	DataTarget* _target;
	SortedFilteredVector<HeapStatItem> _items;
	IGenericListView* _glv;
	IMainFrame* _frame;
};

