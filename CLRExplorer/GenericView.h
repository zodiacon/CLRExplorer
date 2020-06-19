// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GenericListView.h"
#include "Interfaces.h"
#include "ToolBarHelper.h"
#include <atlcoll.h>

class CGenericView : 
	public CFrameWindowImpl<CGenericView, CWindow, CControlWinTraits>,
	public CAutoUpdateUI<CGenericView>,
	public CToolBarHelper<CGenericView> {
public:
	using BaseClass = CFrameWindowImpl<CGenericView, CWindow, CControlWinTraits>;

	DECLARE_WND_CLASS_EX(nullptr, 0, NULL);

	CGenericView(IGenericListViewCallback* listCB, IToolBarProvider* tbCB = nullptr, IDialogBarProvider* dialogBar = nullptr);

	BOOL PreTranslateMessage(MSG* pMsg);

	CCommandBarCtrl m_CmdBar;

	virtual void OnFinalMessage(HWND /*hWnd*/);

	BEGIN_MSG_MAP(CGenericView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMessage)
		CHAIN_MSG_MAP(BaseClass)
		CHAIN_MSG_MAP(CToolBarHelper<CGenericView>)
		REFLECT_NOTIFICATIONS()
	ALT_MSG_MAP(1)
		COMMAND_ID_HANDLER(ID_VIEW_REFRESH, OnRefresh)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnForwardMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CGenericListView m_ListView;
	CToolBarCtrl m_ToolBar;
	IToolBarProvider* m_ToolBarCB;
	IDialogBarProvider* m_DialogBar;
	HWND m_hWndDialogBar{ nullptr };
	CRect m_rcDialogBar;
	IGenericListViewCallback* m_ListViewCB;
};
