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

LRESULT CGenericView::OnCommand(UINT message, WPARAM wParam, LPARAM lParam, BOOL& handled) {
	handled = FALSE;
	if (m_ToolBarCB)
		handled = m_ToolBarCB->OnCommand(LOWORD(wParam));
	LRESULT result = 0;
	if (!handled) {
		handled = ProcessWindowMessage(*this, message, wParam, lParam, result, 1);
	}
	return result;
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


