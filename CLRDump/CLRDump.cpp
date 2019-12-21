// CLRDump.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "..\CLRDiag\DataTarget.h"

void DumpAppDomainsAndAssemblies(DataTarget* dt) {
	auto domains = dt->EnumAppDomains();
	for (auto& ad : domains) {
		printf("AppDomain: %ws (0x%p) (assemblies: %u)\n", (PCWSTR)ad.Name, (void*)ad.Address, ad.AssemblyCount);
		for (auto& asminfo : dt->EnumAssemblies(ad)) {
			printf("\t%ws (0x%p)\n", (PCWSTR)asminfo.Name, (void*)asminfo.Address);
		}
	}
}

void DumpThreadPool(DataTarget* dt) {
	auto data = dt->GetThreadPoolData();
}

void DumpModules(DataTarget* dt, bool includeMethodTables) {
	for (auto& m : dt->EnumModules()) {
		printf("Module 0x%p Asm: 0x%p ID: 0x%p Index: %u IL: 0x%p\n", (void*)m.Address, (void*)m.Assembly, (void*)m.ModuleID, m.ModuleIndex, (void*)m.ILBase);
		if (includeMethodTables) {
			auto mts = dt->EnumMethodTables(m.Address);
			for (auto& mt : mts)
				printf("Index: %u %ws Size: %d Ifaces: %u Methods: %u Virt: %u\n", mt.Index, (PCWSTR)mt.Name, mt.BaseSize, mt.NumInterfaces, mt.NumMethods, mt.NumVirtuals);
		}
	}
}

void DumpThreads(DataTarget* dt) {
	for (auto& data : dt->EnumThreads()) {
		printf("MID: %2d OSID: %5u TEB: 0x%p State: 0x%X\n", data.ManagedThreadId, data.OSThreadId, (void*)data.Teb, data.State);
	}
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

	DumpAppDomainsAndAssemblies(dt.get());
	DumpThreadPool(dt.get());
	DumpThreads(dt.get());
	DumpModules(dt.get(), true);

	if(suspended)
		dt->Resume();

	return 0;
}
