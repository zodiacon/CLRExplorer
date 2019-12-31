#pragma once

#include "Interfaces.h"
#include "DataTarget.h"
#include "SortedFilteredVector.h"
#include "QuickFIlterDialogBar.h"

class SyncBlocksView : public IGenericListViewCallback, public IDialogBarProvider {
public:
	SyncBlocksView(DataTarget* dt);

	void SetFilter(PCWSTR filter);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericListView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;

	// IDialogBarProvider
	HWND Create(HWND hParent) override;

private:
	class CDialogBar : public CQuickFilterDialogBar<CDialogBar> {
	public:
		CDialogBar(SyncBlocksView& view) : BaseClass(IDD_STRINGS_DIALOGBAR), m_View(view) {}
		void ApplyTextFilter(PCWSTR text) {
			m_View.SetFilter(text);
		}
		SyncBlocksView& m_View;
	};

private:
	DataTarget* _target;
	SortedFilteredVector<SyncBlockInfo> _items;
	IGenericListView* _glv;
};

