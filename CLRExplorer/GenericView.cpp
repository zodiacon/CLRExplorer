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
	m_ListView.Create(*this, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_OWNERDATA | LVS_SINGLESEL);
	m_ListView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	m_ToolBar.Create(*this, rcDefault, nullptr,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
		WS_EX_CLIENTEDGE);
	if (m_ToolBarCB && !m_ToolBarCB->Init(m_ToolBar))
		return -1;

	if (m_DialogBar) {
		m_hWndDialogBar = m_DialogBar->Create(*this);
		if (m_hWndDialogBar)
			::ShowWindow(m_hWndDialogBar, SW_SHOW);
	}
	return 0;
}

LRESULT CGenericView::OnSize(UINT, WPARAM, LPARAM lParam, BOOL&) {
	if (m_ListView) {
		int cx = GET_X_LPARAM(lParam), cy = GET_Y_LPARAM(lParam);
		CRect rc;

		if (m_ToolBar.GetButtonCount() == 0) {
			m_ToolBar.ShowWindow(SW_HIDE);
		}
		else {
			m_ToolBar.GetClientRect(&rc);
			m_ToolBar.MoveWindow(0, 0, cx, rc.bottom);
		}
		if (m_hWndDialogBar) {
			if(m_rcDialogBar.IsRectEmpty())
				::GetClientRect(m_hWndDialogBar, &m_rcDialogBar);
			::MoveWindow(m_hWndDialogBar, 0, rc.bottom, cx, m_rcDialogBar.Height(), TRUE);
			rc.bottom += m_rcDialogBar.Height();
		}
		m_ListView.MoveWindow(0, rc.bottom, cx, cy - rc.bottom);
	}
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
	LRESULT result;
	auto msg = reinterpret_cast<MSG*>(lParam);
	handled = ProcessWindowMessage(*this, msg->message, msg->wParam, msg->lParam, result, 1);
	return result;
}

LRESULT CGenericView::OnRefresh(WORD, WORD, HWND, BOOL&) {
	m_ListView.Refresh();

	return 0;
}


