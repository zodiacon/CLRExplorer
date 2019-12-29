#include "pch.h"
#include "..\CLRDiag\DataTarget.h"

CComModule _Module;

void DumpAppDomainsAndAssemblies(DataTarget* dt) {
	auto domains = dt->EnumAppDomains();
	for (auto& ad : domains) {
		printf("AppDomain: %ws (0x%p) (assemblies: %u)\n", (PCWSTR)ad.Name, (void*)ad.AppDomainPtr, ad.AssemblyCount);
		for (auto& asminfo : dt->EnumAssemblies(ad)) {
			printf("\t%ws (0x%p)\n", (PCWSTR)asminfo.Name, (void*)asminfo.AssemblyPtr);
		}
	}
}

void DumpThreadPool(DataTarget* dt) {
	auto data = dt->GetThreadPoolData();
}

void DumpModules(DataTarget* dt, bool includeMethodTables) {
	for (auto& m : dt->EnumModules()) {
		printf("Module 0x%p Asm: 0x%p ID: 0x%p Index: %llu IL: 0x%p\n", (void*)m.Address, (void*)m.Assembly, (void*)m.dwModuleID, m.dwModuleIndex, (void*)m.ilBase);
		if (includeMethodTables) {
			auto mts = dt->EnumMethodTables(m.Address);
			for (auto& mt : mts)
				printf("Index: %u %ws Size: %d Ifaces: %u Methods: %u Virt: %u\n", mt.Index, (PCWSTR)mt.Name, mt.BaseSize, mt.wNumInterfaces, mt.wNumMethods, mt.wNumVirtuals);
		}
	}
}

void DumpThreads(DataTarget* dt) {
	for (auto& data : dt->EnumThreads(false)) {
		printf("MID: %2d OSID: %5u TEB: 0x%p State: 0x%X\n", data.corThreadId, data.osThreadId, (void*)data.teb, data.state);
	}
}

void DumpHandles(DataTarget* dt) {
}

void DumpSyncBlocks(DataTarget* dt) {
	auto sbs = dt->EnumSyncBlocks(false);
}

void DumpObjects(DataTarget* dt) {
	//auto objects = dt->EnumObjects();
	//printf("%zu objects\n", objects.size());
}

int main(int argc, const char* argv[]) {
	if (argc < 2) {
		printf("Usage: CLRDump <pid>\n");
		return 0;
	}

	::CoInitialize(nullptr);

	auto dt = DataTarget::FromProcessId(atoi(argv[1]));
	if (dt == nullptr) {
		printf("Error connecting to target\n");
		return 1;
	}

	bool suspended = dt->Suspend();
	if(!suspended)
		printf("Failed to suspend process\n");

	DumpObjects(dt.get());
	//DumpAppDomainsAndAssemblies(dt.get());
	//DumpThreadPool(dt.get());
	//DumpThreads(dt.get());
	//DumpModules(dt.get(), true);
	//DumpHandles(dt.get());
	//DumpSyncBlocks(dt.get());

	if(suspended)
		dt->Resume();

	return 0;
}
