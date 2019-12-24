#include "pch.h"
#include "TabFactory.h"
#include "Interfaces.h"
#include "SummaryView.h"
#include "GenericView.h"
#include "AssembliesView.h"

HWND TabFactory::CreateTab(DataTarget* dt, DWORD_PTR type, HWND hParent, int& image) {
	HWND hTab = nullptr;
	switch ((NodeType)type) {
		case NodeType::Summary:
		{
			auto impl = new SummaryView(dt);
			auto view = new CGenericView(impl);
			hTab = view->Create(hParent, CRect(), L"Summary", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
			image = 6;
			break;
		}

		case NodeType::AllAssemblies:
		{
			auto impl = new AssembliesView(dt);
			auto view = new CGenericView(impl);
			hTab = view->Create(hParent, CRect(), L"All Assemblies", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
			image = 2;
			break;
		}

	}

	return hTab;
}
