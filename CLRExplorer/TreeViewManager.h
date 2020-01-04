#pragma once

class DataTarget;

#include "Interfaces.h"

class TreeViewManager {
public:
	TreeViewManager(CTreeViewCtrlEx& tv, IMainFrame* frame);
	void InitOnce();
	bool Init(DataTarget* dt, PCWSTR name);
	void DoubleClickNode(HTREEITEM hItem);
	CTreeItem CreateNode(PCWSTR text, int image, DWORD_PTR data, NodeType parent);
	void TabClosed(DWORD_PTR data);
	void CloseAllTabs();

private:
	template<typename T>
	CTreeItem AddNode(PCWSTR text, int image, T data, CTreeItem parent);

private:
	std::unordered_map<DWORD_PTR, CTreeItem> _nodes;
	DataTarget* _target;
	CTreeViewCtrlEx& _tv;
	IMainFrame* _frame;
	CTreeItem _rootNode;
};

template<typename T>
inline CTreeItem TreeViewManager::AddNode(PCWSTR text, int image, T tdata, CTreeItem parent) {
	auto node = parent.AddTail(text, image);
	auto data = static_cast<DWORD_PTR>(tdata);
	node.SetData(data);
	_nodes.insert({ data, node });
	node.EnsureVisible();
	return node;
}
