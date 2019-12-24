#pragma once

#include <atlcomcli.h>
#include <atlstr.h>
#include <clrdata.h>
//#include "CLRDiag.h"
#include "dacprivate.h"

struct AppDomainInfo : DacpAppDomainData {
	CString Name;
};

struct AssemblyInfo : DacpAssemblyData {
	CString Name;
};

struct MethodTableInfo : DacpMethodTableData {
	DWORD Index;
	CString Name;
};

enum class ThreadType {
	Unknown = 0,
	ThreadPoolWorker,
	Finalizer,
};

struct ThreadInfo : DacpThreadData {
	ULONGLONG StackLow;
	ULONGLONG StackHigh;
	ULONGLONG StackCurrent;
	ThreadType Type;
};

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
	std::vector<AssemblyInfo> EnumAssemblies(bool includeSysSharedDomains = false);
	std::vector<DacpModuleData> EnumModules(const DacpAssemblyData& assembly);
	std::vector<DacpModuleData> EnumModules(CLRDATA_ADDRESS assembly);
	std::vector<DacpModuleData> EnumModulesInAppDomain(const DacpAppDomainData& ad);
	std::vector<DacpModuleData> EnumModulesInAppDomain(CLRDATA_ADDRESS addr);
	std::vector<DacpModuleData> EnumModules();

	AppDomainInfo GetSharedDomain();
	AppDomainInfo GetSystemDomain();
	int GetAppDomainCount() const;
	AppDomainInfo GetAppDomainInfo(CLRDATA_ADDRESS addr);
	std::vector<MethodTableInfo> EnumMethodTables(CLRDATA_ADDRESS module);
	DacpGcHeapData GetGCInfo() const;

	DacpGcHeapDetails GetWksHeap();
	std::vector<DacpObjectData> EnumObjects(int gen);

	DacpThreadpoolData GetThreadPoolData();
	std::vector<ThreadInfo> EnumThreads();
	DacpThreadStoreData GetThreadsStats();
	
protected:
	void EnumModulesInternal(CLRDATA_ADDRESS assembly, std::vector<DacpModuleData>& modules);
	void EnumAssembliesInternal(CLRDATA_ADDRESS appDomain, std::vector<AssemblyInfo>& assemblies);

	virtual HRESULT Init() = 0;

protected:
	CComPtr<ICLRDataTarget> _clrTarget;
	CComPtr<IUnknown> _spSos;
};

