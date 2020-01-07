#pragma once

#include "DataTarget.h"
#include "Interfaces.h"
#include "SortedFilteredVector.h"
#include "QuickFIlterDialogBar.h"
#include "resource.h"

class ModulesView : public IGenericListViewCallback, public IDialogBarProvider {
public:
	ModulesView(DataTarget* dt);

	// IDialogBarProvider
	HWND Create(HWND hParent) override;

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericListView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;

	void SetFilter(PCWSTR filter);

private:
	class CDialogBar : public CQuickFilterDialogBar<CDialogBar> {
	public:
		CDialogBar(ModulesView& view) : BaseClass(IDD_STRINGS_DIALOGBAR), m_View(view) {}

		void ApplyTextFilter(PCWSTR text) {
			m_View.SetFilter(text);
		}
		ModulesView& m_View;
	};

private:
	DataTarget* _target;
	SortedFilteredVector<ModuleInfo> _modules;
	IGenericListView* _glv;
};

