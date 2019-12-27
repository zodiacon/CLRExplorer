// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource.h"

#include "aboutdlg.h"
#include "GenericView.h"
#include "MainFrm.h"
#include "ProcessSelectDlg.h"
#include "TabFactory.h"

const int WINDOW_MENU_POSITION = 4;

CMainFrame::CMainFrame() : m_TreeMgr(m_TreeView, this) {
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) {
	if (m_view.PreTranslateMessage(pMsg))
		return TRUE;

	return (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg));
}

BOOL CMainFrame::OnIdle() {
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, nullptr, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// remove old menu
	SetMenu(nullptr);
	m_CmdBar.m_bAlphaImages = true;
	InitCommandBar();

	CToolBarCtrl tb;
	auto hWndToolBar = tb.Create(m_hWnd, nullptr, nullptr, ATL_SIMPLE_TOOLBAR_PANE_STYLE, 0, ATL_IDW_TOOLBAR);
	InitToolBar(tb);
	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, nullptr, TRUE);

	CReBarCtrl(m_hWndToolBar).LockBands(TRUE);
	CreateSimpleStatusBar();

	m_hWndClient = m_Splitter.Create(m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_TreeView.Create(m_Splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_HASLINES |
		TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE, IDC_TREE);
	m_view.Create(m_Splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	m_Splitter.SetSplitterPanes(m_TreeView, m_view);
	UpdateLayout();
	m_Splitter.SetSplitterPos(250);

	m_TreeMgr.InitOnce();
	m_view.SetImageList(m_TreeView.GetImageList());

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != nullptr);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	CMenuHandle menuMain = m_CmdBar.GetMenu();
	m_view.SetWindowMenu(menuMain.GetSubMenu(WINDOW_MENU_POSITION));

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != nullptr);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnWindowClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int nActivePage = m_view.GetActivePage();
	if (nActivePage != -1)
		m_view.RemovePage(nActivePage);
	else
		::MessageBeep((UINT)-1);

	return 0;
}

LRESULT CMainFrame::OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	m_view.RemoveAllPages();

	return 0;
}

LRESULT CMainFrame::OnWindowActivate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int nPage = wID - ID_WINDOW_TABFIRST;
	m_view.SetActivePage(nPage);

	return 0;
}

LRESULT CMainFrame::OnAttachToProcess(WORD, WORD, HWND, BOOL&) {
	CProcessSelectDlg dlg;
	if (dlg.DoModal() == IDOK) {
		CString name;
		auto pid = dlg.GetSelectedProcess(name);
		auto dt = DataTarget::FromProcessId(pid);
		if (dt == nullptr) {
			AtlMessageBox(*this, L"Failed to attach to process", IDS_TITLE);
			return 0;
		}
		m_DataTarget = std::move(dt);
		m_view.RemoveAllPages();
		m_TreeMgr.Init(m_DataTarget.get(), name);
	}
	return 0;
}

LRESULT CMainFrame::OnTreeItemDoubleCLick(int, LPNMHDR, BOOL&) {
	m_TreeMgr.DoubleClickNode(m_TreeView.GetSelectedItem());

	return 0;
}

LRESULT CMainFrame::OnForwardMessage(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int page = m_view.GetActivePage();
	if (page >= 0) {
		auto msg = GetCurrentMessage();
		return ::SendMessage(m_view.GetPageHWND(page), msg->message, msg->wParam, msg->lParam);

	}
	return 0;
}

LRESULT CMainFrame::OnViewTargetData(WORD, WORD id, HWND, BOOL&) {
	auto type = id - ID_TARGET_SUMMARY + 1;
	auto tab = FindTab(type);
	if (tab >= 0)
		SwitchToPage(tab);
	else
		AddTab(type);

	return 0;
}

void CMainFrame::InitCommandBar() {
	struct {
		UINT id, icon;
	} cmds[] = {
//		{ ID_EDIT_COPY, IDI_COPY },
		{ ID_FILE_ATTACHTOPROCESS, IDI_ATTACH },
		{ ID_FILE_OPENDUMPFILE, IDI_OPEN },
		{ ID_TARGET_SUMMARY, IDI_INFO },
		{ ID_TARGET_APPDOMAINS, IDI_PACKAGE },
		{ ID_TARGET_ALLASSEMBLIES, IDI_ASSEMBLY },
		{ ID_TARGET_ALLMODULES, IDI_MODULE },
		{ ID_TARGET_THREADPOOL, IDI_THREADPOOL },
		{ ID_TARGET_THREADS, IDI_THREAD },
		{ ID_WINDOW_CLOSE, IDI_CLOSE },
		{ ID_WINDOW_CLOSE_ALL, IDI_CLOSEALL }
	};
	for (auto& cmd : cmds)
		m_CmdBar.AddIcon(AtlLoadIcon(cmd.icon), cmd.id);
}

void CMainFrame::InitToolBar(CToolBarCtrl& tb) {
	CImageList tbImages;
	tbImages.Create(24, 24, ILC_COLOR32, 8, 4);
	tb.SetImageList(tbImages);

	struct {
		UINT id;
		int image;
		int style = BTNS_BUTTON;
	} buttons[] = {
		{ ID_FILE_ATTACHTOPROCESS, IDI_ATTACH },
		{ ID_FILE_OPENDUMPFILE, IDI_OPEN },
		{ 0 },
		{ ID_TARGET_SUMMARY, IDI_INFO, 0 },
		{ ID_TARGET_APPDOMAINS, IDI_PACKAGE },
		{ ID_TARGET_ALLASSEMBLIES, IDI_ASSEMBLY },
		{ ID_TARGET_ALLMODULES, IDI_MODULE },
		{ ID_TARGET_THREADPOOL, IDI_THREADPOOL },
		{ ID_TARGET_THREADS, IDI_THREAD },
		{ 0 },
		{ ID_WINDOW_CLOSE, IDI_CLOSE },
	};
	for (auto& b : buttons) {
		if (b.id == 0)
			tb.AddSeparator(0);
		else {
			int image = tbImages.AddIcon(AtlLoadIconImage(b.image, 0, 24, 24));
			tb.AddButton(b.id, b.style, TBSTATE_ENABLED, image, nullptr, 0);
		}
	}

}

int CMainFrame::FindTab(DWORD_PTR data) {
	for (int i = 0; i < m_view.GetPageCount(); i++)
		if ((DWORD_PTR)m_view.GetPageData(i) == data)
			return i;

	return -1;
}

void CMainFrame::SwitchToPage(int page) {
	m_view.SetActivePage(page);
}

void CMainFrame::AddTab(DWORD_PTR type) {
	int image;
	auto hTab = TabFactory::CreateTab(m_DataTarget.get(), type, m_view, image);
	if (hTab) {
		WCHAR text[128];
		::GetWindowText(hTab, text, _countof(text));
		m_view.AddPage(hTab, text, image, (PVOID)type);
	}
}
