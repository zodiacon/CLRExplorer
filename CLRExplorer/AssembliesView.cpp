#include "pch.h"
#include "AssembliesView.h"
#include "FormatHelper.h"
#include <algorithm>
#include "SortHelper.h"
#include "resource.h"

AssembliesView::AssembliesView(DataTarget* dt) : _target(dt) {
}

int AssembliesView::GetItemCount() {
	_items.Set(_target->EnumAssemblies(true));
	return static_cast<int>(_items.Size());
}

bool AssembliesView::Init(CListViewCtrl& lv, IGenericListView* glv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 250);
	lv.InsertColumn(1, L"Address", LVCFMT_RIGHT, 130);
	lv.InsertColumn(2, L"AppDomain", LVCFMT_RIGHT, 130);
	lv.InsertColumn(3, L"Class Loader", LVCFMT_RIGHT, 130);
	lv.InsertColumn(4, L"Modules", LVCFMT_CENTER, 60);
	lv.InsertColumn(5, L"Dynamic", LVCFMT_LEFT, 70);
	lv.InsertColumn(6, L"Full Path", LVCFMT_LEFT, 500);

	CImageList images;
	images.Create(16, 16, ILC_COLOR32, 2, 2);
	images.AddIcon(AtlLoadIcon(IDI_ASSEMBLY));
	images.AddIcon(AtlLoadIcon(IDI_ASSEMBLY_DYNAMIC));
	lv.SetImageList(images, LVSIL_SMALL);

	_gv = glv;

	return true;
}

CString AssembliesView::GetItemText(int row, int col) {
	auto& item = _items[row];

	switch (col) {
		case 0: return item.Name.Mid(item.Name.ReverseFind(L'\\') + 1);
		case 1: return FormatHelper::ToHex((PVOID)item.AssemblyPtr);
		case 2: return FormatHelper::ToHex((PVOID)item.BaseDomainPtr);
		case 3: return FormatHelper::ToHex((PVOID)item.ClassLoader);
		case 4: return FormatHelper::ToDec(item.ModuleCount);
		case 5: return item.isDynamic ? L"Yes" : L"No";
		case 6: return item.Name;
	}

	return L"";
}

bool AssembliesView::Sort(int column, bool asc) {
	_items.Sort([=](auto& a1, auto& a2) {
		switch (column) {
			case 0: return SortHelper::SortStrings(a1.Name.Mid(a1.Name.ReverseFind(L'\\') + 1), a2.Name.Mid(a2.Name.ReverseFind(L'\\') + 1), asc);
			case 1: return SortHelper::SortNumbers(a1.AssemblyPtr, a2.AssemblyPtr, asc);
			case 2: return SortHelper::SortNumbers(a1.BaseDomainPtr, a2.BaseDomainPtr, asc);
			case 3: return SortHelper::SortNumbers(a1.ClassLoader, a2.ClassLoader, asc);
			case 4: return SortHelper::SortNumbers(a1.ModuleCount, a2.ModuleCount, asc);
			case 5: return SortHelper::SortNumbers(a1.isDynamic, a2.isDynamic, asc);
			case 6: return SortHelper::SortStrings(a1.Name, a2.Name, asc);
		}
		return false;
		});

	return true;
}

int AssembliesView::GetIcon(int row) {
	return _items[row].isDynamic ? 1 : 0;
}

HWND AssembliesView::Create(HWND hParent) {
	auto dlg = new CDialogBar(*this);
	return dlg->Create(hParent);
}

void AssembliesView::SetFilter(PCWSTR text) {
	CString stext(text);
	if (stext.IsEmpty())
		_items.Filter(nullptr);
	else {
		stext.MakeLower();
		_items.Filter([=](const AssemblyInfo& item) {
			CString temp(item.Name);
			temp.MakeLower();
			return temp.Find(stext) >= 0;
			});
	}
	_gv->SetListViewItemCount(static_cast<int>(_items.FilteredSize()));
}

AssembliesView::CDialogBar::CDialogBar(AssembliesView& view) : 
	BaseClass(IDD_STRINGS_DIALOGBAR), m_View(view) {
}

void AssembliesView::CDialogBar::ApplyTextFilter(PCWSTR text) {
	m_View.SetFilter(text);
}

