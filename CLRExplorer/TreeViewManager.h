#pragma once

class DataTarget;
struct IMainFrame;

class TreeViewManager {
public:
	TreeViewManager(CTreeViewCtrlEx& tv, IMainFrame* frame);
	void InitOnce();
	bool Init(DataTarget* dt, PCWSTR name);
	void DoubleClickNode(HTREEITEM hItem);
	void AddNode(HTREEITEM hItem, HWND hView);
	void RemoveView(HWND hView);

private:
	DataTarget* _target;
	CTreeViewCtrlEx& _tv;
	IMainFrame* _frame;
};

