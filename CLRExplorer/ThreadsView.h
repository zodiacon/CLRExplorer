#pragma once

#include "Interfaces.h"
#include "DataTarget.h"

class ThreadsView : public IGenericListViewCallback, public IToolBarProvider {
public:
	ThreadsView(DataTarget* dt);

	// IGenericListViewCallback
	int GetItemCount() override;
	bool Init(CListViewCtrl& lv, IGenericView* glv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;

	// IToolBarProvider
	bool Init(CToolBarCtrl& tb) override;
	bool OnCommand(WORD id) override;

	static CString ThreadStateToString(DWORD state);

private:
	enum { IdShowDeadThreads = 100 };

	DataTarget* _target;
	std::vector<ThreadInfo> _threads;
	IGenericView* _glv;
	bool _showDeadObjects{ false };
};

