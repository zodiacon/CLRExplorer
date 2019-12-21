#include "pch.h"
#include "TreeViewManager.h"
#include "DataTarget.h"
#include "resource.h"

TreeViewManager::TreeViewManager(CTreeViewCtrlEx& tv) : _tv(tv) {
}

void TreeViewManager::InitOnce() {
	CImageList images;
	images.Create(16, 16, ILC_COLOR32, 16, 8);

	UINT icons[] = {
		IDI_APP, IDI_PACKAGE, IDI_ASSEMBLY, IDI_MODULE, IDI_THREADPOOL, IDI_THREAD, IDI_INFO
	};

	for (auto id : icons)
		images.AddIcon(AtlLoadIcon(id));

	_tv.SetImageList(images, TVSIL_NORMAL);
}

bool TreeViewManager::Init(DataTarget* dt, PCWSTR name) {
	_tv.DeleteAllItems();
	_tv.LockWindowUpdate();

	// init nodes
	CString rootText;
	rootText.Format(L"%s (%u)", name, dt->GetProcessId());
	auto root = _tv.InsertItem(rootText, 0, 0, TVI_ROOT, TVI_LAST);
	auto summary = root.AddTail(L"Summary", 6);
	auto appDomainsNode = root.InsertAfter(L"AppDomains", root, 1);
	appDomainsNode.SetData((DWORD_PTR)NodeType::AppDomains);
	auto assemblies = root.AddTail(L"All Assemblies", 2);
	auto modules = root.AddTail(L"All Modules", 3);
	auto threadPool = root.AddTail(L"Thread Pool", 4);
	auto thread = root.AddTail(L"Threads", 5);

	root.Expand(TVE_EXPAND);

	_tv.LockWindowUpdate(FALSE);
	return true;
}

void TreeViewManager::DoubleClickNode(HTREEITEM hItem) {
	auto node(hItem);
}
