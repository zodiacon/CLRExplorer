#include "pch.h"
#include "TabFactory.h"
#include "Interfaces.h"
#include "SummaryView.h"
#include "GenericView.h"
#include "AssembliesView.h"
#include "SyncBlocksView.h"
#include "ThreadsView.h"
#include "StringsView.h"
#include "ObjectsView.h"
#include "HeapStatsView.h"
#include "ObjectsView.h"
#include "ModulesView.h"

constexpr DWORD viewStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

HWND TabFactory::CreateTab(DataTarget* dt, DWORD_PTR type, HWND hParent, IMainFrame* frame, int& image, NodeType genericType) {
	if (genericType != NodeType::None)
		return CreateSpecificTab(dt, type, genericType, hParent, frame, image);

	HWND hTab = nullptr;
	
	switch ((NodeType)type) {
		case NodeType::Summary:
		{
			auto impl = new SummaryView(dt);
			auto view = new CGenericView(impl);
			hTab = view->Create(hParent, CRect(), L"Summary", viewStyle);
			image = 6;
			break;
		}

		case NodeType::AllAssemblies:
		{
			auto impl = new AssembliesView(dt);
			auto view = new CGenericView(impl, nullptr, impl);
			hTab = view->Create(hParent, CRect(), L"All Assemblies", viewStyle);
			image = 2;
			break;
		}

		case NodeType::AllModules:
		{
			auto impl = new ModulesView(dt);
			auto view = new CGenericView(impl, nullptr, impl);
			hTab = view->Create(hParent, CRect(), L"All Modules", viewStyle);
			image = 3;
			break;
		}

		case NodeType::SyncBlocks:
		{
			auto impl = new SyncBlocksView(dt);
			auto view = new CGenericView(impl, nullptr, impl);
			hTab = view->Create(hParent, CRect(), L"Sync Blocks", viewStyle);
			image = 7;
			break;
		}

		case NodeType::HeapStats:
		{
			auto impl = new HeapStatsView(dt, frame);
			auto view = new CGenericView(impl, nullptr, impl);
			hTab = view->Create(hParent, CRect(), L"Heap Stats", viewStyle);
			image = 10;
			break;
		}

		case NodeType::Threads:
		{
			auto impl = new ThreadsView(dt);
			auto view = new CGenericView(impl, impl);
			hTab = view->Create(hParent, CRect(), L"Threads", viewStyle);
			image = 5;
			break;
		}

		case NodeType::AllStrings:
		{
			auto impl = new StringsView(dt);
			auto view = new CGenericView(impl, nullptr, impl);
			hTab = view->Create(hParent, CRect(), L"All Strings", viewStyle);
			image = 9;
			break;
		}
	}

	return hTab;
}

HWND TabFactory::CreateSpecificTab(DataTarget* dt, DWORD_PTR type, NodeType genericType, HWND hParent, IMainFrame* frame, int& image) {
	HWND hTab = nullptr;

	switch (genericType) {
		case NodeType::MethodTable:
			auto impl = new ObjectsView(dt, type);	// type is MT address
			auto view = new CGenericView(impl);
			hTab = view->Create(hParent, CRect(), L"Instances (" + impl->GetMethodTable().Name + L")", viewStyle);
			image = 11;
			break;
	}
	return hTab;
}
