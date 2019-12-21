#pragma once

#include "CLRDiag.h"

using ModuleMapTraverse = void(__stdcall*)(DWORD index, ULONGLONG methodTable, LPARAM param);

struct __declspec(uuid("5c552ab6-fc09-4cb3-8e36-22fa03c798b7")) IXCLRDataProcess : IUnknown {
};

struct __declspec(uuid("436f00f2-b42a-4b9f-870c-e73db66ae930")) ISosDac : IUnknown {
	// ThreadStore
	virtual HRESULT __stdcall GetThreadStoreData(ThreadStoreData* data) = 0;

	// AppDomains
	virtual HRESULT __stdcall GetAppDomainStoreData(AppDomainStoreData* data) = 0;
	virtual HRESULT __stdcall GetAppDomainList(DWORD count, CLRDATA_ADDRESS* buffer, DWORD* needed) = 0;
	virtual HRESULT __stdcall GetAppDomainData(CLRDATA_ADDRESS addr, AppDomainData* data) = 0;
	virtual HRESULT __stdcall GetAppDomainName(CLRDATA_ADDRESS addr, DWORD count, PWSTR buffer, DWORD* needed) = 0;
	virtual HRESULT __stdcall GetDomainFromContext(ULONGLONG context, CLRDATA_ADDRESS* appDomain) = 0;

	// Assemblies
	virtual HRESULT __stdcall GetAssemblyList(CLRDATA_ADDRESS appDomain, DWORD count, CLRDATA_ADDRESS* buffer, DWORD* needed) = 0;
	virtual HRESULT __stdcall GetAssemblyData(CLRDATA_ADDRESS appDomain, CLRDATA_ADDRESS assembly, AssemblyData* data) = 0;
	virtual HRESULT __stdcall GetAssemblyName(CLRDATA_ADDRESS addr, ULONG count, PWSTR name, DWORD* needed) = 0;

	// Modules
	virtual HRESULT __stdcall GetMetaDataImport(ULONGLONG addr, IMetaDataImport** ppImport) = 0;
	virtual HRESULT __stdcall GetModuleData(CLRDATA_ADDRESS module, ModuleData* data) = 0;
	virtual HRESULT __stdcall TraverseModuleMap(int mmt, CLRDATA_ADDRESS module, ModuleMapTraverse callback, LPARAM param) = 0;
	virtual HRESULT __stdcall GetAssemblyModuleList(CLRDATA_ADDRESS addr, ULONG count, CLRDATA_ADDRESS* buffer, DWORD* needed) = 0;
	virtual HRESULT __stdcall GetILForModule(CLRDATA_ADDRESS moduleAddr, DWORD rva, ULONGLONG* il) = 0;

	// Threads

	virtual HRESULT __stdcall GetThreadData(CLRDATA_ADDRESS addr, ThreadData* data) = 0;
	virtual HRESULT __stdcall GetThreadFromThinlockID(DWORD thinLockId, CLRDATA_ADDRESS* thread) = 0;
	virtual HRESULT __stdcall GetStackLimits(CLRDATA_ADDRESS thread, ULONGLONG* lower, ULONGLONG* upper, ULONGLONG* fp) = 0;

	// MethodDescs

	virtual HRESULT __stdcall GetMethodDescData(ULONGLONG md, ULONGLONG ip, MethodDescData* data, DWORD revertedRejitVersions, RejitData*, DWORD* needed) = 0;
	virtual HRESULT __stdcall GetMethodDescPtrFromIP(ULONGLONG ip, ULONGLONG* md) = 0;
	virtual HRESULT __stdcall GetMethodDescName(ULONGLONG md, DWORD count, PWSTR name, DWORD* needed) = 0;
	virtual HRESULT __stdcall GetMethodDescPtrFromFrame(ULONGLONG frame, ULONGLONG* md) = 0;
	virtual HRESULT __stdcall GetMethodDescFromToken(ULONGLONG module, mdToken token, ULONGLONG* methodDesc) = 0;
	virtual HRESULT __stdcall GetMethodDescTransparencyData_dont_use();

	// JIT Data
	virtual HRESULT __stdcall GetCodeHeaderData(ULONGLONG ip, CodeHeaderData* data) = 0;
	virtual HRESULT __stdcall GetJitManagerList();
	virtual HRESULT __stdcall GetJitHelperFunctionName();
	virtual HRESULT __stdcall GetJumpThunkTarget();

	// ThreadPool

	virtual HRESULT __stdcall GetThreadpoolData(ThreadPoolData* data) = 0;
	virtual HRESULT __stdcall GetWorkRequestData(ULONGLONG request, WorkRequestData* data) = 0;
	virtual HRESULT __stdcall GetHillClimbingLogEntry_dont_use();

	// Objects
	virtual HRESULT __stdcall GetObjectData(CLRDATA_ADDRESS addr, V45ObjectData* data) = 0;
	virtual HRESULT __stdcall GetObjectStringData(CLRDATA_ADDRESS addr, DWORD count, PWSTR data, DWORD* needed) = 0;
	virtual HRESULT __stdcall GetObjectClassName(CLRDATA_ADDRESS addr, DWORD count, PWSTR data, DWORD* needed) = 0;

	// MethodTable
	virtual HRESULT __stdcall GetMethodTableName(ULONGLONG mt, DWORD count, PWSTR name, DWORD* needed) = 0;
	virtual HRESULT __stdcall GetMethodTableData(ULONGLONG mt, MethodTableData* data) = 0;
	virtual HRESULT __stdcall GetMethodTableSlot(ULONGLONG mt, DWORD slot, ULONGLONG* value) = 0;
	virtual HRESULT __stdcall GetMethodTableFieldData(ULONGLONG mt, V4FieldInfo* data);
	virtual HRESULT __stdcall GetMethodTableTransparencyData_dont_use();

	// EEClass
	virtual HRESULT __stdcall GetMethodTableForEEClass(ULONGLONG eeclass, ULONGLONG* mt) = 0;

	// FieldDesc
	virtual HRESULT __stdcall GetFieldDescData(ULONGLONG fd, FieldData* data) = 0;

	// Frames
	virtual HRESULT __stdcall GetFrameName(ULONGLONG vtable, DWORD count, PWSTR name, DWORD* needed) = 0;

	// PEFiles
	virtual HRESULT __stdcall GetPEFileBase(ULONGLONG addr, ULONGLONG* baseAddress) = 0;
	virtual HRESULT __stdcall GetPEFileName(ULONGLONG addr, DWORD count, PWSTR name, DWORD* needed) = 0;

	// GC
	virtual HRESULT __stdcall GetGCHeapData(GCInfo* info) = 0;
	virtual HRESULT __stdcall GetGCHeapList(DWORD count, ULONGLONG* heaps, DWORD* needed) = 0; // svr only
	virtual HRESULT __stdcall GetGCHeapDetails(ULONGLONG addr, HeapDetails* details) = 0; // wks only
	virtual HRESULT __stdcall GetGCHeapStaticData(HeapDetails* details) = 0;
	virtual HRESULT __stdcall GetHeapSegmentData(ULONGLONG segment, SegmentData* data);
	virtual HRESULT __stdcall GetOOMData_dont_use();
	virtual HRESULT __stdcall GetOOMStaticData_dont_use();
	virtual HRESULT __stdcall GetHeapAnalyzeData_dont_use();
	virtual HRESULT __stdcall GetHeapAnalyzeStaticData_dont_use();

	// DomainLocal
	virtual HRESULT __stdcall GetDomainLocalModuleData_dont_use();
	virtual HRESULT __stdcall GetDomainLocalModuleDataFromAppDomain();
	virtual HRESULT __stdcall GetDomainLocalModuleDataFromModule();

	// ThreadLocal
	virtual HRESULT __stdcall GetThreadLocalModuleData();

	// SyncBlock
	virtual HRESULT __stdcall GetSyncBlockData(DWORD index, SyncBlockData* data) = 0;
	virtual HRESULT __stdcall GetSyncBlockCleanupData();

	// Handles
	virtual HRESULT __stdcall GetHandleEnum(IUnknown** ppEnum) = 0;
	virtual HRESULT __stdcall GetHandleEnumForTypes(ClrHandleKind* types, int count, IUnknown** ppEnum) = 0;
	virtual HRESULT __stdcall GetHandleEnumForGC(int gen, IEnumUnknown** ppEnum);

	// EH
	virtual HRESULT __stdcall TraverseEHInfo_dont_use();
	virtual HRESULT __stdcall GetNestedExceptionData(ULONGLONG addr, CLRDATA_ADDRESS* exception, ULONGLONG* nextNestedException) = 0;

	// StressLog
	virtual HRESULT __stdcall GetStressLogAddress(ULONGLONG* addr) = 0;

	// Heaps
	virtual HRESULT __stdcall TraverseLoaderHeap(ULONGLONG loaderHeapAddr, PVOID callback) = 0;
	virtual HRESULT __stdcall GetCodeHeapList();
	virtual HRESULT __stdcall TraverseVirtCallStubHeap();

	// Other
	virtual HRESULT __stdcall GetUsefulGlobals(CommonMethodTables* data);
	virtual HRESULT __stdcall GetClrWatsonBuckets();
	virtual HRESULT __stdcall GetTLSIndex(DWORD* index) = 0;
	virtual HRESULT __stdcall GetDacModuleHandle(HMODULE* pModule);

	// COM
	virtual HRESULT __stdcall GetRCWData(ULONGLONG addr, RCWData* data) = 0;
	virtual HRESULT __stdcall GetRCWInterfaces();
	virtual HRESULT __stdcall GetCCWData(ULONGLONG addr, CCWData* data) = 0;
	virtual HRESULT __stdcall GetCCWInterfaces();
	virtual HRESULT __stdcall TraverseRCWCleanupList_dont_use();

	// GC Reference Functions
	virtual HRESULT __stdcall GetStackReferences(DWORD osThreadId, IEnumUnknown** ppEnum) = 0;
	virtual HRESULT __stdcall GetRegisterName(int regName, DWORD count, PWSTR name, DWORD* needed) = 0;
	virtual HRESULT __stdcall GetThreadAllocData();
	virtual HRESULT __stdcall GetHeapAllocData();

	// For BindingDisplay plugin

	virtual HRESULT __stdcall GetFailedAssemblyList();
	virtual HRESULT __stdcall GetPrivateBinPaths();
	virtual HRESULT __stdcall GetAssemblyLocation();
	virtual HRESULT __stdcall GetAppDomainConfigFile();
	virtual HRESULT __stdcall GetApplicationBase();
	virtual HRESULT __stdcall GetFailedAssemblyData();
	virtual HRESULT __stdcall GetFailedAssemblyLocation();
	virtual HRESULT __stdcall GetFailedAssemblyDisplayName();
};
