#pragma once

#include <atlcomcli.h>
#include <atlstr.h>
#include <clrdata.h>
#include "CLRDiag.h"

class DataTarget abstract {
public:
	static std::unique_ptr<DataTarget> FromProcessId(DWORD pid);
	static std::unique_ptr<DataTarget> FromDumpFile(PCWSTR dumpFilePath);

	virtual DWORD GetProcessId() const = 0;
	virtual bool Suspend() {
		return false;
	}
	virtual bool Resume() {
		return false;
	}

	std::vector<AppDomainInfo> EnumAppDomains();
	std::vector<AssemblyInfo> EnumAssemblies(AppDomainInfo& ad);
	std::vector<AssemblyInfo> EnumAssemblies(CLRDATA_ADDRESS appDomainAddress);
	std::vector<AssemblyInfo> EnumAssemblies();
	std::vector<ModuleData> EnumModules(const AssemblyData& assembly);
	std::vector<ModuleData> EnumModules(CLRDATA_ADDRESS assembly);
	std::vector<ModuleData> EnumModulesInAppDomain(const AppDomainData& ad);
	std::vector<ModuleData> EnumModulesInAppDomain(CLRDATA_ADDRESS addr);
	std::vector<ModuleData> EnumModules();
	AppDomainInfo GetSharedDomain();
	AppDomainInfo GetSystemDomain();
	AppDomainInfo GetAppDomainInfo(CLRDATA_ADDRESS addr);
	std::vector<MethodTableInfo> EnumMethodTables(CLRDATA_ADDRESS module);

	HeapDetails GetWksHeap();
	std::vector<V45ObjectData> EnumObjects(int gen);

	ThreadPoolData GetThreadPoolData();
	std::vector<ThreadInfo> EnumThreads();
	ThreadStoreData GetThreadsStats();
	
protected:
	void EnumModulesInternal(CLRDATA_ADDRESS assembly, std::vector<ModuleData>& modules);
	void EnumAssembliesInternal(CLRDATA_ADDRESS appDomain, std::vector<AssemblyInfo>& assemblies);

	virtual HRESULT Init() = 0;

protected:
	CComPtr<ICLRDataTarget> _clrTarget;
	CComPtr<IUnknown> _spSos;
};

