#include "pch.h"
#include "ObjectsView.h"

ObjectsView::ObjectsView(DataTarget* dt, const MethodTableInfo& mt) : _target(dt), _mt(mt) {
}

int ObjectsView::GetItemCount() {

	return 0;
}

bool ObjectsView::Init(CListViewCtrl& lv, IGenericListView* glv) {
	return false;
}

CString ObjectsView::GetItemText(int row, int col) {
	return CString();
}

bool ObjectsView::Sort(int column, bool ascending) {
	return false;
}
