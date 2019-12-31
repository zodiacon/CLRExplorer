#pragma once

#include <atlcomcli.h>
#include <atlstr.h>
#include <clrdata.h>
#include <unordered_map>
#include <functional>
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

struct SyncBlockInfo : DacpSyncBlockData {
	int Index;
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

struct ObjectInfo : DacpObjectData {
	CLRDATA_ADDRESS Address;
	int Generation;
	CString StringValue;
};

struct HeapStatItem {
	CLRDATA_ADDRESS MethodTable;
	unsigned ObjectCount;
	long long TotalSize;
	DacpObjectType Type;
	CString TypeName;
};

using EnumObjectCallback = std::function<bool(ObjectInfo& obj, void* userData)>;

class DataTarget abstract {
public:
	static std::unique_ptr<DataTarget> FromProcessId(DWORD pid);
	static std::unique_ptr<DataTarget> FromDumpFile(PCWSTR dumpFilePath);

	virtual DWORD GetProcessId() const = 0;
	virtual CString GetProcessPathName() = 0;
	virtual FILETIME GetProcessStartTime() = 0;

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
	std::vector<SyncBlockInfo> EnumSyncBlocks(bool includeFree);
	bool EnumObjects(EnumObjectCallback callback, void* userData = nullptr);
	std::vector<HeapStatItem> GetHeapStats(CLRDATA_ADDRESS address = 0);

	DacpThreadData GetThreadData(CLRDATA_ADDRESS addr);
	CString GetObjectClassName(CLRDATA_ADDRESS address);
	CString GetObjectString(CLRDATA_ADDRESS address, unsigned maxLength = 256);
	DacpMethodTableData GetMethodTableInfo(CLRDATA_ADDRESS mt);

	AppDomainInfo GetSharedDomain();
	AppDomainInfo GetSystemDomain();
	int GetAppDomainCount() const;
	AppDomainInfo GetAppDomainInfo(CLRDATA_ADDRESS addr);
	std::vector<MethodTableInfo> EnumMethodTables(CLRDATA_ADDRESS module);
	DacpGcHeapData GetGCInfo() const;

	DacpGcHeapDetails GetWksHeap();

	DacpThreadpoolData GetThreadPoolData();
	std::vector<ThreadInfo> EnumThreads(bool includeDeadThreads);
	DacpThreadStoreData GetThreadsStats();
	
protected:
	bool EnumObjectsInternal(DacpGcHeapDetails& details, EnumObjectCallback callback, void* userData);
	void EnumModulesInternal(CLRDATA_ADDRESS assembly, std::vector<DacpModuleData>& modules);
	void EnumAssembliesInternal(CLRDATA_ADDRESS appDomain, std::vector<AssemblyInfo>& assemblies);

	virtual HRESULT Init() = 0;

protected:
	CComPtr<ICLRDataTarget> _clrTarget;
	CComPtr<IUnknown> _spSos;
	std::unordered_map<CLRDATA_ADDRESS, MethodTableInfo> _mtCache;
};

