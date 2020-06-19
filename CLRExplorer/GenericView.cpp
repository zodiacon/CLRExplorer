// View.cpp : implementation of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource.h"

#include "GenericView.h"

CGenericView::CGenericView(IGenericListViewCallback* listCB, IToolBarProvider* tbCB, IDialogBarProvider* db)
	: m_ListViewCB(listCB), m_ToolBarCB(tbCB), m_ListView(listCB), m_DialogBar(db) {
}

BOOL CGenericView::PreTranslateMessage(MSG* pMsg) {
	return FALSE;
}

void CGenericView::OnFinalMessage(HWND /*hWnd*/) {
	delete this;
}

LRESULT CGenericView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	m_hWndClient = m_ListView.Create(*this, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_OWNERDATA | LVS_SINGLESEL);
	m_ListView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_INFOTIP);

	auto hWndToolBar = m_ToolBar.Create(m_hWnd, rcDefault, nullptr, ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST, 0, ATL_IDW_TOOLBAR);
	m_ToolBar.SetExtendedStyle(TBSTYLE_EX_MIXEDBUTTONS);

	if (m_ToolBarCB && !m_ToolBarCB->Init(m_ToolBar))
		return -1;

	if (m_DialogBar) {
		m_hWndDialogBar = m_DialogBar->Create(m_ToolBar);
		if (m_hWndDialogBar) {
			CImageList tbImages;
			tbImages.Create(24, 24, ILC_COLOR32, 4, 4);
			m_ToolBar.SetImageList(tbImages);

			::ShowWindow(m_hWndDialogBar, SW_SHOW);
			auto id = 300;
			if (m_ToolBarCB == nullptr)
				m_ToolBar.AddButton(id, BTNS_BUTTON, TBSTATE_ENABLED, I_IMAGENONE, nullptr, 0);
			CreateToolbarControl(m_ToolBar, m_hWndDialogBar, id);
		}
	}
	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(m_ToolBar);
	CReBarCtrl(m_hWndToolBar).LockBands(TRUE);
	UIAddToolBar(hWndToolBar);

	return 0;
}

LRESULT CGenericView::OnCommand(UINT message, WPARAM wParam, LPARAM lParam, BOOL& handled) {
	handled = FALSE;
	if (m_ToolBarCB)
		handled = m_ToolBarCB->OnCommand(LOWORD(wParam));
	if (!handled && m_DialogBar) {
		handled = m_DialogBar->HandleCommand(LOWORD(wParam));
		if (!handled && m_hWndDialogBar)
			handled = !::SendMessage(m_hWndDialogBar, WM_COMMAND, wParam, lParam);
	}
	return 0;
}

LRESULT CGenericView::OnForwardMessage(UINT, WPARAM, LPARAM lParam, BOOL& handled) {
	LRESULT result = 0;
	auto msg = reinterpret_cast<MSG*>(lParam);
	handled = ProcessWindowMessage(*this, msg->message, msg->wParam, msg->lParam, result, 1);
	return result;
}

LRESULT CGenericView::OnRefresh(WORD, WORD, HWND, BOOL&) {
	m_ListView.Refresh();

	return 0;
}


