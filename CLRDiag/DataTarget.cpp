#include "pch.h"
#include "DataTarget.h"
#include "CLRDiag.h"
#include "LiveProcessDataTarget.h"
#include "sos.h"

//CComModule _Module;

std::unique_ptr<DataTarget> DataTarget::FromProcessId(DWORD pid) {
	std::unique_ptr<DataTarget> target = std::make_unique<LiveProcessDataTarget>(pid);
	return target->Init() == S_OK ? std::move(target) : nullptr;
}

std::vector<AppDomainInfo> DataTarget::EnumAppDomains() {
	std::vector<AppDomainInfo> domains;

	CComQIPtr<ISosDac> spSos(_spSos);
	CLRDATA_ADDRESS addr[64];
	DWORD needed;
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
	return EnumAssemblies(ad.Address);
}

std::vector<ModuleData> DataTarget::EnumModules(const AssemblyData& assembly) {
	return EnumModules(assembly.Address);
}

std::vector<ModuleData> DataTarget::EnumModules(CLRDATA_ADDRESS assembly) {
	std::vector<ModuleData> modules;
	EnumModulesInternal(assembly, modules);
	return modules;
}

std::vector<ModuleData> DataTarget::EnumModulesInAppDomain(const AppDomainData& ad) {
	return EnumModulesInAppDomain(ad.Address);
}

void DataTarget::EnumModulesInternal(CLRDATA_ADDRESS assembly, std::vector<ModuleData>& modules) {
	CComQIPtr<ISosDac> spSos(_spSos);
	CLRDATA_ADDRESS addr[16];
	DWORD count;
	auto hr = spSos->GetAssemblyModuleList(assembly, _countof(addr), addr, &count);
	if (FAILED(hr))
		return;

	modules.reserve(count);
	for (DWORD i = 0; i < count; i++) {
		ModuleData data;
		spSos->GetModuleData(addr[i], &data);
		modules.push_back(data);
	}
}

void DataTarget::EnumAssembliesInternal(CLRDATA_ADDRESS appDomain, std::vector<AssemblyInfo>& assemblies) {
	CComQIPtr<ISosDac> spSos(_spSos);
	CLRDATA_ADDRESS addr[512];
	DWORD count;
	if (FAILED(spSos->GetAssemblyList(appDomain, _countof(addr), addr, &count)))
		return;

	WCHAR name[512];
	DWORD len;
	for (DWORD i = 0; i < count; i++) {
		AssemblyInfo data;
		if(FAILED(spSos->GetAssemblyData(appDomain, addr[i], &data)))
			continue;
		spSos->GetAssemblyName(addr[i], _countof(name), name, &len);
		data.Name = name;

		assemblies.push_back(std::move(data));
	}
}

std::vector<ModuleData> DataTarget::EnumModulesInAppDomain(CLRDATA_ADDRESS addr) {
	std::vector<ModuleData> modules;
	CComQIPtr<ISosDac> spSos(_spSos);
	CLRDATA_ADDRESS assemblies[512];
	DWORD count;
	if (FAILED(spSos->GetAssemblyList(addr, _countof(assemblies), assemblies, &count)))
		return modules;

	modules.reserve(count);
	for (DWORD i = 0; i < count; i++) {
		EnumModulesInternal(assemblies[i], modules);
	}
	return modules;
}

std::vector<ModuleData> DataTarget::EnumModules() {
	CComQIPtr<ISosDac> spSos(_spSos);
	std::vector<ModuleData> modules;
	CLRDATA_ADDRESS addr[16];
	DWORD needed;
	auto hr = spSos->GetAppDomainList(_countof(addr), addr, &needed);
	if (FAILED(hr))
		return modules;

	DWORD count;
	CLRDATA_ADDRESS assemblies[512];
	for (DWORD i = 0; i < needed; i++) {
		spSos->GetAssemblyList(addr[i], _countof(assemblies), assemblies, &count);
		for (DWORD j = 0; j < count; j++)
			EnumModulesInternal(assemblies[j], modules);
	}
	return modules;
}

AppDomainInfo DataTarget::GetSharedDomain() {
	CComQIPtr<ISosDac> spSos(_spSos);
	AppDomainStoreData data;
	auto hr = spSos->GetAppDomainStoreData(&data);
	ATLASSERT(SUCCEEDED(hr));
	return GetAppDomainInfo(data.SharedDomain);
}

AppDomainInfo DataTarget::GetSystemDomain() {
	CComQIPtr<ISosDac> spSos(_spSos);
	AppDomainStoreData data;
	spSos->GetAppDomainStoreData(&data);
	return GetAppDomainInfo(data.SystemDomain);
}

AppDomainInfo DataTarget::GetAppDomainInfo(CLRDATA_ADDRESS addr) {
	CComQIPtr<ISosDac> spSos(_spSos);
	AppDomainInfo data;
	WCHAR name[MAX_PATH];
	DWORD len;
	spSos->GetAppDomainData(addr, &data);
	spSos->GetAppDomainName(addr, _countof(name), name, &len);
	data.Name = name;
	return data;
}

std::vector<MethodTableInfo> DataTarget::EnumMethodTables(CLRDATA_ADDRESS module) {
	CComQIPtr<ISosDac> spSos(_spSos);
	std::vector<std::pair<ULONGLONG, DWORD>> addr;
	addr.reserve(1024);
	spSos->TraverseModuleMap(0, module, [](auto index, auto mt, auto param) {
		auto vec = (std::vector<std::pair<ULONGLONG, DWORD>>*)param;
		vec->push_back({ mt, index });
		}, (LPARAM)&addr);

	std::vector<MethodTableInfo> mts;
	mts.reserve(addr.size());
	WCHAR name[512];
	DWORD count;
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

HeapDetails DataTarget::GetWksHeap() {
	CComQIPtr<ISosDac> spSos(_spSos);
	HeapDetails details = { 0 };
	spSos->GetGCHeapStaticData(&details);
	return details;
}

std::vector<V45ObjectData> DataTarget::EnumObjects(int gen) {
	return std::vector<V45ObjectData>();
}

std::vector<AssemblyInfo> DataTarget::EnumAssemblies(CLRDATA_ADDRESS appDomainAddress) {
	std::vector<AssemblyInfo> assemblies;

	CComQIPtr<ISosDac> spSos(_spSos);
	CLRDATA_ADDRESS addr[1024];
	DWORD count;
	auto hr = spSos->GetAssemblyList(appDomainAddress, _countof(addr), addr, &count);
	if (FAILED(hr))
		return assemblies;

	EnumAssembliesInternal(appDomainAddress, assemblies);

	return assemblies;
}

std::vector<AssemblyInfo> DataTarget::EnumAssemblies() {
	std::vector<AssemblyInfo> assemblies;
	CComQIPtr<ISosDac> spSos(_spSos);
	CLRDATA_ADDRESS addr[64];
	DWORD needed;
	auto hr = spSos->GetAppDomainList(_countof(addr), addr, &needed);
	if (FAILED(hr))
		return assemblies;
	assemblies.reserve(64);
	for (DWORD i = 0; i < needed; i++) {
		EnumAssembliesInternal(addr[i], assemblies);
	}
	return assemblies;
}

ThreadPoolData DataTarget::GetThreadPoolData() {
	CComQIPtr<ISosDac> spSos(_spSos);
	ThreadPoolData data;
	spSos->GetThreadpoolData(&data);
	return data;
}

std::vector<ThreadInfo> DataTarget::EnumThreads() {
	std::vector<ThreadInfo> threads;
	CComQIPtr<ISosDac> spSos(_spSos);
	ATLASSERT(spSos);

	auto stat = GetThreadsStats();
	threads.reserve(stat.ThreadCount + 10);
	for (auto addr = stat.FirstThread; addr; ) {
		ThreadInfo data;
		spSos->GetThreadData(addr, &data);
		spSos->GetStackLimits(addr, &data.StackLow, &data.StackHigh, &data.StackCurrent);
		threads.emplace_back(data);
		addr = data.NextThread;
	}
	return threads;
}

ThreadStoreData DataTarget::GetThreadsStats() {
	CComQIPtr<ISosDac> spSos(_spSos);
	ATLASSERT(spSos);

	ThreadStoreData stat;
	spSos->GetThreadStoreData(&stat);

	return stat;
}

