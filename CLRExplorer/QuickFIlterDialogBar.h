#pragma once

#include "resource.h"

template<typename T>
class CQuickFilterDialogBar : public CDialogImpl<T> {
public:
	using BaseClass = CQuickFilterDialogBar<T>;

	BEGIN_MSG_MAP(BaseClass)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnColorDialog)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnColorDialog)
		COMMAND_HANDLER(IDC_TEXT, EN_CHANGE, OnTextChanged)
		COMMAND_HANDLER(IDC_CLEAR, BN_CLICKED, OnClearText)
		COMMAND_ID_HANDLER(ID_EDIT_FIND, OnFind)
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_KEYDOWN, OnEditKeyDown)
	END_MSG_MAP()

	int IDD;		// dialog ID

	CQuickFilterDialogBar(UINT idd) : IDD(idd), m_Edit(this, 1) {
	}

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

private:
	LRESULT OnEditKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled) {
		if (wParam == VK_ESCAPE) {
			m_Edit.SetWindowTextW(L"");
			bHandled = TRUE;
			return 0;
		}
		return DefWindowProc();
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
		m_Edit.SubclassWindow(GetDlgItem(IDC_TEXT));

		return TRUE;
	}

	LRESULT OnColorDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
		return (LRESULT)::GetSysColorBrush(COLOR_WINDOW);
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM id, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
		if (id == 100) {
			KillTimer(100);
			CString text;
			GetDlgItemText(IDC_TEXT, text);
			static_cast<T*>(this)->ApplyTextFilter(text);
		}
		return 0;
	}

	LRESULT OnTextChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		SetTimer(100, 350);
		return 0;
	}

	LRESULT OnClearText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		SetDlgItemText(IDC_TEXT, L"");
		return 0;
	}

	LRESULT OnFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		GetDlgItem(IDC_TEXT).SetFocus();
		return 0;
	}

protected:
	CContainedWindow m_Edit;
};
