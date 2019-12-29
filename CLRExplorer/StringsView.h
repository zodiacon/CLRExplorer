#pragma once

#include "Interfaces.h"
#include "DataTarget.h"
#include "resource.h"

class StringsView : public IGenericListViewCallback, public IDialogBarProvider {
public:
	StringsView(DataTarget* dt);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;

	// IGenericDialogBarProvider
	HWND Create(HWND hParent) override;

	void ApplyTextFilter(PCWSTR filter);

private:
	class CDialogBar : public CDialogImpl<CDialogBar> {
	public:
		CDialogBar(StringsView& view) : m_View(view) {}

		enum { IDD = IDD_STRINGS_DIALOGBAR };

		BEGIN_MSG_MAP(CAboutDlg)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			MESSAGE_HANDLER(WM_TIMER, OnTimer)
			MESSAGE_HANDLER(WM_CTLCOLORDLG, OnColorDialog)
			MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorDialog)
			COMMAND_CODE_HANDLER(EN_CHANGE, OnTextChanged)
		END_MSG_MAP()

		// Handler prototypes (uncomment arguments if needed):
		//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	private:
		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnColorDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		LRESULT OnTextChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	private:
		StringsView& m_View;
	};

	static bool CompareItems(const ObjectInfo& str1, const ObjectInfo& str2, int col, bool asc);
	void GetStringValue(ObjectInfo& obj);

private:
	DataTarget* _target;
	CDialogBar* _dialogBar;
	std::vector<ObjectInfo> _strings, _allStrings;
	IGenericView* _gv;
};

