#include "pch.h"
#include "TabFactory.h"
#include "Interfaces.h"
#include "SummaryView.h"
#include "GenericView.h"
#include "AssembliesView.h"
#include "SyncBlocksView.h"
#include "ThreadsView.h"

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

		case NodeType::SyncBlocks:
		{
			auto impl = new SyncBlocksView(dt);
			auto view = new CGenericView(impl);
			hTab = view->Create(hParent, CRect(), L"Sync Blocks", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
			image = 7;
			break;
		}

		case NodeType::Threads:
		{
			auto impl = new ThreadsView(dt);
			auto view = new CGenericView(impl, impl);
			hTab = view->Create(hParent, CRect(), L"Threads", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
			image = 5;
			break;
		}

	}

	return hTab;
}
