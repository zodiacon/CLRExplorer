#include "pch.h"
#include "resource.h"
#include "ModulesView.h"
#include "FormatHelper.h"
#include "SortHelper.h"

ModulesView::ModulesView(DataTarget* dt) : _target(dt) {
}

HWND ModulesView::Create(HWND hParent) {
	auto dlg = new CDialogBar(*this);
	return dlg->Create(hParent);
}

int ModulesView::GetItemCount() {
	_modules.Set(_target->EnumModules());

	return static_cast<int>(_modules.FilteredSize());
}

bool ModulesView::Init(CListViewCtrl& lv, IGenericListView* glv) {
	_glv = glv;

	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 230);
	lv.InsertColumn(1, L"Address", LVCFMT_RIGHT, 100);
	lv.InsertColumn(2, L"Assembly", LVCFMT_RIGHT, 100);
	lv.InsertColumn(3, L"File Name", LVCFMT_LEFT, 300);

	CImageList images;
	images.Create(16, 16, ILC_COLOR32, 2, 0);
	images.AddIcon(AtlLoadIconImage(IDI_MODULE, 0, 16, 16));
	lv.SetImageList(images, LVSIL_SMALL);

	return true;
}

CString ModulesView::GetItemText(int row, int col) {
	auto& item = _modules[row];
	switch (col) {
		case 0: return item.Name;
		case 1: return FormatHelper::ToHex(item.Address);
		case 2: return FormatHelper::ToHex(item.Assembly);
		case 3: return item.FileName;
	}
	return L"";
}

bool ModulesView::Sort(int column, bool ascending) {
	_modules.Sort([=](auto& m1, auto& m2) {
		switch (column) {
			case 0: return SortHelper::SortStrings(m1.Name, m2.Name, ascending);
			case 1: return SortHelper::SortNumbers(m1.Address, m2.Address, ascending);
			case 2: return SortHelper::SortNumbers(m1.Assembly, m2.Assembly , ascending);
			case 3: return SortHelper::SortStrings(m1.FileName, m2.FileName, ascending);
		}
		return false;
		});
	return true;
}

int ModulesView::GetIcon(int row) {
	return 0;
}

void ModulesView::SetFilter(PCWSTR filter) {
	CString sfilter(filter);
	sfilter.MakeUpper();
	_modules.Filter([&](auto& m) {
		CString name(m.Name);
		name.MakeUpper();
		CString filename(m.FileName);
		filename.MakeUpper();
		return name.Find(sfilter) >= 0 || filename.Find(sfilter) >= 0;
		});

	_glv->SetListViewItemCount(static_cast<int>(_modules.FilteredSize()));
}
