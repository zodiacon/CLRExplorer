#pragma once

class DataTarget;

class TreeViewManager {
public:
	TreeViewManager(CTreeViewCtrlEx& tv);
	void InitOnce();
	bool Init(DataTarget* dt, PCWSTR name);
	void DoubleClickNode(HTREEITEM hItem);
	void AddNode(HTREEITEM hItem, HWND hView);
	void RemoveView(HWND hView);

	enum class NodeType {
		Root,
		AppDomains,
		AllAssemblies,
		AllModules,
		ThreadPool,
		Threads
	};

private:
	DataTarget* m_Target;
	CTreeViewCtrlEx& _tv;
	std::unordered_map<HWND, CTreeItem> m_Nodes;
};

