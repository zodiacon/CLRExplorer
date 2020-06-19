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
		IDI_SYNCBLK, IDI_MEMORY, IDI_STRING, IDI_HEAP_STATS, IDI_OBJECT
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
	_rootNode = root;

	auto summary = AddNode(L"Summary", 6, NodeType::Summary, root);
	auto appDomainsNode = AddNode(L"AppDomains", 1, NodeType::AppDomains, root);
	auto assemblies = AddNode(L"Assemblies", 2, NodeType::AllAssemblies, root);
	auto modules = AddNode(L"Modules", 3, NodeType::AllModules, root);
	auto heapStats = AddNode(L"Heap Stats", 10, NodeType::HeapStats, root);
	auto threads = AddNode(L"Threads", 5, NodeType::Threads, root);
	auto threadPool = AddNode(L"Thread Pool", 4, NodeType::ThreadPool, root);
	auto syncBlocks = AddNode(L"Sync Blocks", 7, NodeType::SyncBlocks, root);
	auto strings = AddNode(L"Strings", 9, NodeType::AllStrings, root);

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

CTreeItem TreeViewManager::CreateNode(PCWSTR text, int image, DWORD_PTR data, NodeType parent) {
	auto node = _nodes[(DWORD_PTR)parent];
	auto newNode = AddNode(text, image, data, node);
	return newNode;
}

void TreeViewManager::TabClosed(DWORD_PTR data) {
	auto node = _nodes[data];
	ATLASSERT(node);
	if (node.GetParent() != _rootNode) {
		node.Delete();
		_nodes.erase(data);
	}
}

void TreeViewManager::CloseAllTabs() {
	std::vector<DWORD_PTR> items;
	for (auto& [data, node] : _nodes) {
		if (node.GetParent() != _rootNode) {
			node.Delete();
			items.push_back(data);
		}
	}
	for (auto data : items)
		_nodes.erase(data);
}
