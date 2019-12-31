#pragma once

#include "Interfaces.h"
#include "DataTarget.h"
#include "resource.h"
#include "QuickFIlterDialogBar.h"

class StringsView : public IGenericListViewCallback, public IDialogBarProvider {
public:
	StringsView(DataTarget* dt);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericListView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;

	// IGenericDialogBarProvider
	HWND Create(HWND hParent) override;

	void SetFilter(PCWSTR filter);

private:
	class CDialogBar : public CQuickFilterDialogBar<CDialogBar> {
	public:
		CDialogBar(StringsView& view) : BaseClass(IDD_STRINGS_DIALOGBAR), m_View(view) {}

		void ApplyTextFilter(PCWSTR text) {
			m_View.SetFilter(text);
		}

	private:
		StringsView& m_View;
	};

	static bool CompareItems(const ObjectInfo& str1, const ObjectInfo& str2, int col, bool asc);
	void GetStringValue(ObjectInfo& obj);

private:
	DataTarget* _target;
	CDialogBar* _dialogBar;
	std::vector<ObjectInfo> _allStrings;
	std::vector<int> _strings;
	IGenericListView* _gv;
};

