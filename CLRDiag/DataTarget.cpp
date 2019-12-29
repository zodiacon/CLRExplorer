#include "pch.h"
#include "DataTarget.h"
#include "LiveProcessDataTarget.h"
#include "dacprivate.h"

//CComModule _Module;


inline static size_t Align(size_t nbytes) {
#ifdef _WIN64
	return (nbytes + 7) & ~7;
#else
	return (nbytes + 3) & ~3;
#endif
}

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
	for (int i = 0; i < count; i++) {
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
	for (int i = 0; i < count; i++) {
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
		for (int j = 0; j < count; j++)
			EnumModulesInternal(assemblies[j], modules);
	}
	return modules;
}

std::vector<SyncBlockInfo> DataTarget::EnumSyncBlocks(bool includeFree) {
	std::vector<SyncBlockInfo> sbs;

	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	SyncBlockInfo data;
	auto hr = spSos->GetSyncBlockData(1, &data);
	if (FAILED(hr))	// no sync blocks
		return sbs;

	sbs.reserve(64);
	if (includeFree || !data.bFree) {
		data.Index = 1;
		sbs.push_back(data);
	}
	auto count = data.SyncBlockCount;
	for (UINT i = 1; i <= count; i++) {
		spSos->GetSyncBlockData(i, &data);
		if (includeFree || !data.bFree) {
			data.Index = i;
			sbs.push_back(data);
		}
	}

	return sbs;
}

DacpThreadData DataTarget::GetThreadData(CLRDATA_ADDRESS addr) {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	DacpThreadData data;
	spSos->GetThreadData(addr, &data);
	return data;
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
	spSos->TraverseModuleMap(TYPEDEFTOMETHODTABLE, module, [](UINT index, CLRDATA_ADDRESS mt, PVOID param) {
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

bool DataTarget::EnumObjects(EnumObjectCallback callback, void* userData) {
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	DacpGcHeapData data;
	spSos->GetGCHeapData(&data);
	CLRDATA_ADDRESS heaps[256];
	unsigned count;
	DacpGcHeapDetails details;
	HRESULT hr;
	if (data.bServerMode) {
		hr = spSos->GetGCHeapList(_countof(heaps), heaps, &count);
		if (FAILED(hr))
			return false;
		for (unsigned i = 0; i < count; i++) {
			hr = spSos->GetGCHeapDetails(heaps[i], &details);
			ATLASSERT(SUCCEEDED(hr));
			if (!EnumObjectsInternal(details, callback, userData))
				break;
		}
	}
	else {
		hr = spSos->GetGCHeapStaticData(&details);
		if (FAILED(hr))
			return false;
		EnumObjectsInternal(details, callback, userData);
	}
	return true;
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

std::vector<ThreadInfo> DataTarget::EnumThreads(bool includeDeadThreads) {
	std::vector<ThreadInfo> threads;
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	ATLASSERT(spSos);

	auto stat = GetThreadsStats();
	threads.reserve(stat.threadCount + 10);
	for (auto addr = stat.firstThread; addr; ) {
		ThreadInfo data;
		spSos->GetThreadData(addr, &data);
		if (includeDeadThreads || data.osThreadId > 0) {
			spSos->GetStackLimits(addr, &data.StackLow, &data.StackHigh, &data.StackCurrent);
			threads.emplace_back(data);
		}
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

constexpr int min_obj_size = sizeof(BYTE*) + sizeof(PVOID) + sizeof(size_t);

bool DataTarget::EnumObjectsInternal(DacpGcHeapDetails& heap, EnumObjectCallback callback, void* userData) {
	DacpHeapSegmentData segdata;
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	ObjectInfo obj;
	WCHAR text[256];

	auto gen0start = heap.generation_table[0].allocation_start;
	auto gen0end = heap.alloc_allocated;
	auto segment = heap.generation_table[2].start_segment;
	auto currentSeg = segment;
	auto hr = segdata.Request(spSos, segment, heap);
	ATLASSERT(SUCCEEDED(hr));

	auto current = segdata.mem;
	bool prevFree = false;

	for (;;) {
		auto segmentEnd = heap.alloc_allocated;
		if (currentSeg == heap.ephemeral_heap_segment) {
			if (current - sizeof(PVOID) == gen0end - Align(min_obj_size))
				break;
		}
		if (current >= segmentEnd) {
			if (current > segmentEnd)
				break;

			segment = segdata.next;
			if (segment) {
				if (segdata.Request(spSos, segment, heap) != S_OK)
					break;
				current = segdata.mem;
				currentSeg = segment;
			}
			else
				break;
		}

		if (currentSeg == heap.ephemeral_heap_segment && current >= gen0end) {
			// something is wrong
			break;
		}
		hr = spSos->GetObjectData(current, &obj);
		if (FAILED(hr))
			break;

		obj.Address = current;
		//obj.Generation = gen;
		if (!callback(obj, userData))
			return true;
		current += Align(obj.Size);
	}

	// LOH

	segment = heap.generation_table[3].start_segment;
	currentSeg = segment;
	if (segdata.Request(spSos, segment, heap) != S_OK)
		return false;

	current = segdata.mem;
	for (;;) {
		auto segmentEnd = segdata.used;
		if (current >= segmentEnd) {
			if (current > segmentEnd)
				break;

			segment = segdata.next;
			if (segment) {
				if (segdata.Request(spSos, segment, heap) != S_OK)
					break;
				current = segdata.mem;
				currentSeg = segment;
			}
			else
				break;
		}

		hr = spSos->GetObjectData(current, &obj);
		if (FAILED(hr))
			break;

		if (obj.ObjectType == OBJ_STRING) {
			unsigned len;
			hr = spSos->GetObjectStringData(current, _countof(text), text, &len);
			ATLASSERT(SUCCEEDED(hr));
			obj.StringValue = text;
		}
		obj.Address = current;
		obj.Generation = 3;
		if (!callback(obj, userData))
			return true;
		current += Align(obj.Size);
	}
	return true;
}

CString DataTarget::GetObjectClassName(CLRDATA_ADDRESS address) {
	WCHAR className[512];
	unsigned len;
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	auto hr = spSos->GetObjectClassName(address, _countof(className), className, &len);
	return hr == S_OK ? className : L"";
}

CString DataTarget::GetObjectString(CLRDATA_ADDRESS address, unsigned maxLength) {
	auto buffer = std::make_unique<WCHAR[]>(maxLength);
	CComQIPtr<ISOSDacInterface> spSos(_spSos);
	unsigned len;
	auto hr = spSos->GetObjectStringData(address, maxLength, buffer.get(), &len);
	return FAILED(hr) ? L"" : buffer.get();
}
