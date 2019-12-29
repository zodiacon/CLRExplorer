#include "pch.h"
#include "TreeViewManager.h"
#include "DataTarget.h"
#include "resource.h"
#include "Interfaces.h"

TreeViewManager::TreeViewManager(CTreeViewCtrlEx& tv, IMainFrame* frame) : _tv(tv), _frame(frame) {
}

void TreeViewManager::InitOnce() {
	CImageList images;
	int size = 16;
	images.Create(size, size, ILC_COLOR32, 16, 8);

	UINT icons[] = {
		IDI_APP, IDI_PACKAGE, IDI_ASSEMBLY, IDI_MODULE, IDI_THREADPOOL, IDI_THREAD, IDI_INFO,
		IDI_SYNCBLK, IDI_MEMORY, IDI_STRING
	};

	for (auto id : icons)
		images.AddIcon(AtlLoadIconImage(id, 0, size, size));

	_tv.SetImageList(images, TVSIL_NORMAL);
}

bool TreeViewManager::Init(DataTarget* dt, PCWSTR name) {
	_tv.LockWindowUpdate();
	_tv.DeleteAllItems();

	// init nodes
	CString rootText;
	rootText.Format(L"%s (%u)", name, dt->GetProcessId());
	auto root = _tv.InsertItem(rootText, 0, 0, TVI_ROOT, TVI_LAST);
	auto summary = root.AddTail(L"Summary", 6);
	summary.SetData((DWORD_PTR)NodeType::Summary);
	auto appDomainsNode = root.InsertAfter(L"AppDomains", root, 1);
	appDomainsNode.SetData((DWORD_PTR)NodeType::AppDomains);
	auto assemblies = root.AddTail(L"All Assemblies", 2);
	assemblies.SetData((DWORD_PTR)NodeType::AllAssemblies);
	auto modules = root.AddTail(L"All Modules", 3);
	auto heaps = root.AddTail(L"GC Heaps", 8);
	heaps.SetData((DWORD_PTR)NodeType::GCHeaps);
	auto threads = root.AddTail(L"Threads", 5);
	threads.SetData((DWORD_PTR)NodeType::Threads);
	auto threadPool = root.AddTail(L"Thread Pool", 4);
	auto syncBlocks = root.AddTail(L"Sync Blocks", 7);
	syncBlocks.SetData((DWORD_PTR)NodeType::SyncBlocks);
	auto strings = root.AddTail(L"All Strings", 9);
	strings.SetData((DWORD_PTR)NodeType::AllStrings);

	root.Expand(TVE_EXPAND);

	_tv.LockWindowUpdate(FALSE);

	_frame->AddTab((DWORD_PTR)NodeType::Summary);
	return true;
}

void TreeViewManager::DoubleClickNode(HTREEITEM hItem) {
	auto data = _tv.GetItemData(hItem);
	auto tab = _frame->FindTab(data);
	if (tab >= 0)
		_frame->SwitchToPage(tab);
	else {
		_frame->AddTab(data);
	}
}
