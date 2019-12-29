#include "pch.h"
#include "ObjectsView.h"

ObjectsView::ObjectsView(DataTarget* dt) : _target(dt) {
}

int ObjectsView::GetItemCount() {
	return 0;
}

bool ObjectsView::Init(CListViewCtrl& lv, IGenericView* glv) {
	return false;
}

CString ObjectsView::GetItemText(int row, int col) {
	return CString();
}

bool ObjectsView::Sort(int column, bool ascending) {
	return false;
}
