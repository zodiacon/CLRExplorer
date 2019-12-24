#include "pch.h"
#include "DataTarget.h"
//#include "CLRDiag.h"
#include "LiveProcessDataTarget.h"
//#include "sos.h"
//#include "sospriv.h"
#include "dacprivate.h"

//CComModule _Module;

std::unique_ptr<DataTarget> DataTarget::FromProcessId(DWORD pid) {
	std::unique_ptr<DataTarget> target = std::make_unique<LiveProcessDataTarget>(pid);
	return target->Init() == S_OK ? std::move(target) : nullptr;
}

std::vector<AppDomainInfo> DataTarget::EnumAppDomains() {
	std::vector<AppDomainInfo> domains;

	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	CLRDATA_ADDRESS addr[64];
	unsigned needed;
	auto hr = spSos->GetAppDomainList(_countof(addr), addr, &needed);
	if (FAILED(hr))
		return domains;

	domains.reserve(needed);
	for (DWORD i = 0; i < needed; i++) {
		domains.push_back(GetAppDomainInfo(addr[i]));
	}

	return domains;
}

std::vector<AssemblyInfo> DataTarget::EnumAssemblies(AppDomainInfo& ad) {
	return EnumAssemblies(ad.AppDomainPtr);
}

std::vector<DacpModuleData> DataTarget::EnumModules(const DacpAssemblyData& assembly) {
	return EnumModules(assembly.AssemblyPtr);
}

std::vector<DacpModuleData> DataTarget::EnumModules(CLRDATA_ADDRESS assembly) {
	std::vector<DacpModuleData> modules;
	EnumModulesInternal(assembly, modules);
	return modules;
}

std::vector<DacpModuleData> DataTarget::EnumModulesInAppDomain(const DacpAppDomainData& ad) {
	return EnumModulesInAppDomain(ad.AppDomainPtr);
}

void DataTarget::EnumModulesInternal(CLRDATA_ADDRESS assembly, std::vector<DacpModuleData>& modules) {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	CLRDATA_ADDRESS addr[16];
	unsigned count;
	auto hr = spSos->GetAssemblyModuleList(assembly, _countof(addr), addr, &count);
	if (FAILED(hr))
		return;

	modules.reserve(count);
	for (DWORD i = 0; i < count; i++) {
		DacpModuleData data;
		spSos->GetModuleData(addr[i], &data);
		modules.push_back(data);
	}
}

void DataTarget::EnumAssembliesInternal(CLRDATA_ADDRESS appDomain, std::vector<AssemblyInfo>& assemblies) {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	CLRDATA_ADDRESS addr[512];
	int count;
	if (FAILED(spSos->GetAssemblyList(appDomain, _countof(addr), addr, &count)))
		return;

	WCHAR name[512];
	unsigned len;
	for (DWORD i = 0; i < count; i++) {
		AssemblyInfo data;
		if (FAILED(spSos->GetAssemblyData(appDomain, addr[i], &data)))
			continue;
		spSos->GetAssemblyName(addr[i], _countof(name), name, &len);
		data.Name = name;

		assemblies.push_back(std::move(data));
	}
}

std::vector<DacpModuleData> DataTarget::EnumModulesInAppDomain(CLRDATA_ADDRESS addr) {
	std::vector<DacpModuleData> modules;
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	CLRDATA_ADDRESS assemblies[512];
	int count;
	if (FAILED(spSos->GetAssemblyList(addr, _countof(assemblies), assemblies, &count)))
		return modules;

	modules.reserve(count);
	for (DWORD i = 0; i < count; i++) {
		EnumModulesInternal(assemblies[i], modules);
	}
	return modules;
}

std::vector<DacpModuleData> DataTarget::EnumModules() {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	std::vector<DacpModuleData> modules;
	CLRDATA_ADDRESS addr[16];
	unsigned needed;
	auto hr = spSos->GetAppDomainList(_countof(addr), addr, &needed);
	if (FAILED(hr))
		return modules;

	int count;
	CLRDATA_ADDRESS assemblies[512];
	for (DWORD i = 0; i < needed; i++) {
		spSos->GetAssemblyList(addr[i], _countof(assemblies), assemblies, &count);
		for (DWORD j = 0; j < count; j++)
			EnumModulesInternal(assemblies[j], modules);
	}
	return modules;
}

AppDomainInfo DataTarget::GetSharedDomain() {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	DacpAppDomainStoreData data;
	auto hr = spSos->GetAppDomainStoreData(&data);
	ATLASSERT(SUCCEEDED(hr));
	return GetAppDomainInfo(data.sharedDomain);
}

AppDomainInfo DataTarget::GetSystemDomain() {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	DacpAppDomainStoreData data;
	spSos->GetAppDomainStoreData(&data);
	return GetAppDomainInfo(data.systemDomain);
}

int DataTarget::GetAppDomainCount() const {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	DacpAppDomainStoreData data;
	spSos->GetAppDomainStoreData(&data);
	return data.DomainCount;
}

AppDomainInfo DataTarget::GetAppDomainInfo(CLRDATA_ADDRESS addr) {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	AppDomainInfo data;
	WCHAR name[MAX_PATH];
	unsigned len;
	spSos->GetAppDomainData(addr, &data);
	spSos->GetAppDomainName(addr, _countof(name), name, &len);
	data.Name = name;
	return data;
}

std::vector<MethodTableInfo> DataTarget::EnumMethodTables(CLRDATA_ADDRESS module) {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	std::vector<std::pair<ULONGLONG, DWORD>> addr;
	addr.reserve(1024);
	spSos->TraverseModuleMap(TYPEDEFTOMETHODTABLE, module, [](auto index, auto mt, auto param) {
		auto vec = (std::vector<std::pair<ULONGLONG, DWORD>>*)param;
		vec->push_back({ mt, index });
		}, &addr);

	std::vector<MethodTableInfo> mts;
	mts.reserve(addr.size());
	WCHAR name[512];
	unsigned count;
	for (auto& [mt, index] : addr) {
		MethodTableInfo data;
		spSos->GetMethodTableData(mt, &data);
		data.Index = index;
		if (S_OK == spSos->GetMethodTableName(mt, _countof(name), name, &count))
			data.Name = name;
		mts.push_back(data);
	}

	return mts;
}

DacpGcHeapData DataTarget::GetGCInfo() const {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	DacpGcHeapData info;
	spSos->GetGCHeapData(&info);
	return info;
}

DacpGcHeapDetails DataTarget::GetWksHeap() {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	DacpGcHeapDetails details;
	spSos->GetGCHeapStaticData(&details);
	return details;
}

std::vector<DacpObjectData> DataTarget::EnumObjects(int gen) {
	return std::vector<DacpObjectData>();
}

std::vector<AssemblyInfo> DataTarget::EnumAssemblies(CLRDATA_ADDRESS appDomainAddress) {
	std::vector<AssemblyInfo> assemblies;

	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	CLRDATA_ADDRESS addr[1024];
	int count;
	auto hr = spSos->GetAssemblyList(appDomainAddress, _countof(addr), addr, &count);
	if (FAILED(hr))
		return assemblies;

	EnumAssembliesInternal(appDomainAddress, assemblies);

	return assemblies;
}

std::vector<AssemblyInfo> DataTarget::EnumAssemblies(bool includeSysSharedDomains) {
	std::vector<AssemblyInfo> assemblies;
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	CLRDATA_ADDRESS addr[64];
	unsigned needed;
	auto hr = spSos->GetAppDomainList(_countof(addr), addr, &needed);
	if (FAILED(hr))
		return assemblies;
	assemblies.reserve(64);
	for (DWORD i = 0; i < needed; i++) {
		EnumAssembliesInternal(addr[i], assemblies);
	}
	if (includeSysSharedDomains) {
		DacpAppDomainStoreData data;
		spSos->GetAppDomainStoreData(&data);
		EnumAssembliesInternal(data.sharedDomain, assemblies);
		EnumAssembliesInternal(data.systemDomain, assemblies);
	}
	return assemblies;
}

DacpThreadpoolData DataTarget::GetThreadPoolData() {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	DacpThreadpoolData data;
	spSos->GetThreadpoolData(&data);
	return data;
}

std::vector<ThreadInfo> DataTarget::EnumThreads() {
	std::vector<ThreadInfo> threads;
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	ATLASSERT(spSos);

	auto stat = GetThreadsStats();
	threads.reserve(stat.threadCount + 10);
	for (auto addr = stat.firstThread; addr; ) {
		ThreadInfo data;
		spSos->GetThreadData(addr, &data);
		spSos->GetStackLimits(addr, &data.StackLow, &data.StackHigh, &data.StackCurrent);
		threads.emplace_back(data);
		addr = data.nextThread;
	}
	return threads;
}

DacpThreadStoreData DataTarget::GetThreadsStats() {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	ATLASSERT(spSos);

	DacpThreadStoreData stat;
	spSos->GetThreadStoreData(&stat);

	return stat;
}

