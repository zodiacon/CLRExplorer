// View.cpp : implementation of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource.h"

#include "GenericView.h"

CGenericView::CGenericView(IGenericListViewCallback* listCB, IToolBarProvider* tbCB) 
	: m_ListViewCB(listCB), m_ToolBarCB(tbCB), m_ListView(listCB) {
}

BOOL CGenericView::PreTranslateMessage(MSG* pMsg) {
	pMsg;
	return FALSE;
}

void CGenericView::OnFinalMessage(HWND /*hWnd*/) {
	delete this;
}

LRESULT CGenericView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	m_ListView.Create(*this, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_OWNERDATA | LVS_SINGLESEL);
	m_ListView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	if (!m_ListViewCB->Init(m_ListView))
		return -1;

	m_ToolBar.Create(*this, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	if (m_ToolBarCB && !m_ToolBarCB->Init(m_ToolBar))
		return -1;

	return 0;
}

LRESULT CGenericView::OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&) {
	return 1;
}

LRESULT CGenericView::OnSize(UINT, WPARAM, LPARAM lParam, BOOL&) {
	if (m_ListView) {
		int cx = GET_X_LPARAM(lParam), cy = GET_Y_LPARAM(lParam);
		if (m_ToolBar.GetButtonCount() == 0) {
			m_ListView.MoveWindow(0, 0, cx, cy);
			m_ToolBar.ShowWindow(SW_HIDE);
		}
		else {
			CRect rc;
			m_ToolBar.GetClientRect(&rc);
			m_ToolBar.MoveWindow(0, 0, cx, rc.bottom);
			m_ListView.MoveWindow(0, rc.bottom, cx, cy - rc.bottom);
		}
	}
	return 0;
}

LRESULT CGenericView::OnCommand(UINT, WPARAM wParam, LPARAM, BOOL& handled) {
	if (m_ToolBarCB)
		handled = m_ToolBarCB->OnCommand(LOWORD(wParam));
	return 0;
}


