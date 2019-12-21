#pragma once

enum class ClrHandleKind {
	/// <summary>
	/// Weak, short lived handle.
	/// </summary>
	WeakShort = 0,

	/// <summary>
	/// Weak, long lived handle.
	/// </summary>
	WeakLong = 1,

	/// <summary>
	/// Strong handle.
	/// </summary>
	Strong = 2,

	/// <summary>
	/// Strong handle, prevents relocation of target object.
	/// </summary>
	Pinned = 3,

	/// <summary>
	/// RefCounted handle (strong when the reference count is greater than 0).
	/// </summary>
	RefCount = 5,

	/// <summary>
	/// A weak handle which may keep its "secondary" object alive if the "target" object is also alive.
	/// </summary>
	Dependent = 6,

	/// <summary>
	/// A strong, pinned handle (keeps the target object from being relocated), used for async IO operations.
	/// </summary>
	AsyncPinned = 7,

	/// <summary>
	/// Strong handle used internally for book keeping.
	/// </summary>
	SizedRef = 8
};

struct ModuleData {
	CLRDATA_ADDRESS Address;
	CLRDATA_ADDRESS PEFile;
	CLRDATA_ADDRESS ILBase;
	CLRDATA_ADDRESS MetadataStart;
	ULONGLONG MetadataSize;
	CLRDATA_ADDRESS Assembly;
	DWORD IsReflection;
	DWORD IsPEFile;
	ULONGLONG BaseClassIndex;
	ULONGLONG ModuleID;
	DWORD TransientFlags;
	CLRDATA_ADDRESS TypeDefToMethodTableMap;
	CLRDATA_ADDRESS TypeRefToMethodTableMap;
	CLRDATA_ADDRESS MethodDefToDescMap;
	CLRDATA_ADDRESS FieldDefToDescMap;
	CLRDATA_ADDRESS MemberRefToDescMap;
	CLRDATA_ADDRESS FileReferencesMap;
	CLRDATA_ADDRESS ManifestModuleReferencesMap;
	CLRDATA_ADDRESS LookupTableHeap;
	CLRDATA_ADDRESS ThunkHeap;
	DWORD ModuleIndex;
};

struct RejitData {
	CLRDATA_ADDRESS RejitID;
	DWORD Flags;
	CLRDATA_ADDRESS NativeCodeAddr;
};

struct MethodDescData {
	DWORD HasNativeCode;
	DWORD IsDynamic;
	short SlotNumber;
	CLRDATA_ADDRESS NativeCodeAddr;

	// Useful for breaking when a method is jitted.
	CLRDATA_ADDRESS AddressOfNativeCodeSlot;

	CLRDATA_ADDRESS MethodDesc;
	CLRDATA_ADDRESS MethodTable;
	CLRDATA_ADDRESS Module;

	DWORD Token;
	CLRDATA_ADDRESS GCInfo;
	CLRDATA_ADDRESS GCStressCodeCopy;

	// This is only valid if bIsDynamic is true
	CLRDATA_ADDRESS ManagedDynamicMethodObject;

	CLRDATA_ADDRESS RequestedIP;

	// Gives info for the single currently active version of a method
	RejitData RejitDataCurrent;

	// Gives info corresponding to requestedIP (for !ip2md)
	RejitData RejitDataRequested;

	// Total number of rejit versions that have been jitted
	DWORD JittedRejitVersions;
};

struct GenerationData {
	CLRDATA_ADDRESS StartSegment;
	CLRDATA_ADDRESS AllocationStart;

	// These are examined only for generation 0, otherwise NULL
	CLRDATA_ADDRESS AllocationContextPointer;
	CLRDATA_ADDRESS AllocationContextLimit;
};

struct HeapDetails {
	ULONGLONG Address; // Only filled in in server mode, otherwise NULL
	ULONGLONG Allocated;
	ULONGLONG MarkArray;
	ULONGLONG CAllocateLH;
	ULONGLONG NextSweepObj;
	ULONGLONG SavedSweepEphemeralSeg;
	ULONGLONG SavedSweepEphemeralStart;
	ULONGLONG BackgroundSavedLowestAddress;
	ULONGLONG BackgroundSavedHighestAddress;

	GenerationData GenerationTable[4];
	ULONGLONG EphemeralHeapSegment;

	ULONGLONG FinalizationFillPointers[7];
	ULONGLONG LowestAddress;
	ULONGLONG HighestAddress;
	ULONGLONG CardTable;
};

struct GCInfo {
	int ServerMode;
	int GCStructuresValid;
	int HeapCount;
	int MaxGeneration;
};

struct AssemblyData {
	CLRDATA_ADDRESS Address;
	CLRDATA_ADDRESS ClassLoader;
	CLRDATA_ADDRESS ParentDomain;
	CLRDATA_ADDRESS AppDomain;
	CLRDATA_ADDRESS AssemblySecurityDescriptor;
	int Dynamic;
	int ModuleCount;
	DWORD LoadContext;
	int IsDomainNeutral;
	DWORD LocationFlags;
};

struct FieldData {
	DWORD ElementType; // CorElementType
	DWORD SigType; // CorElementType
	CLRDATA_ADDRESS TypeMethodTable; // NULL if Type is not loaded
	CLRDATA_ADDRESS TypeModule;
	DWORD TypeToken;
	DWORD FieldToken;
	CLRDATA_ADDRESS MTOfEnclosingClass;
	DWORD Offset;
	DWORD IsThreadLocal;
	DWORD IsContextLocal;
	DWORD IsStatic;
	CLRDATA_ADDRESS NextField;
};

struct AppDomainData {
	CLRDATA_ADDRESS Address;
	CLRDATA_ADDRESS SecurityDescriptor;
	CLRDATA_ADDRESS LowFrequencyHeap;
	CLRDATA_ADDRESS HighFrequencyHeap;
	CLRDATA_ADDRESS StubHeap;
	CLRDATA_ADDRESS DomainLocalBlock;
	CLRDATA_ADDRESS DomainLocalModules;
	int Id;
	int AssemblyCount;
	int FailedAssemblyCount;
	int Stage;
};

struct ThreadStoreData {
	int ThreadCount;
	int UnstartedThreadCount;
	int BackgroundThreadCount;
	int PendingThreadCount;
	int DeadThreadCount;
	CLRDATA_ADDRESS FirstThread;
	CLRDATA_ADDRESS FinalizerThread;
	CLRDATA_ADDRESS GCThread;
	DWORD HostConfig;
};

struct ThreadLocalModuleData {
	CLRDATA_ADDRESS ThreadAddress;
	CLRDATA_ADDRESS ModuleIndex;

	CLRDATA_ADDRESS ClassData;
	CLRDATA_ADDRESS DynamicClassTable;
	CLRDATA_ADDRESS GCStaticDataStart;
	CLRDATA_ADDRESS NonGCStaticDataStart;
};


struct ThreadData {
	DWORD ManagedThreadId;
	DWORD OSThreadId;
	int State;
	DWORD PreemptiveGCDisabled;
	CLRDATA_ADDRESS AllocationContextPointer;
	CLRDATA_ADDRESS AllocationContextLimit;
	CLRDATA_ADDRESS Context;
	CLRDATA_ADDRESS Domain;
	CLRDATA_ADDRESS Frame;
	DWORD LockCount;
	CLRDATA_ADDRESS FirstNestedException;
	CLRDATA_ADDRESS Teb;
	CLRDATA_ADDRESS FiberData;
	CLRDATA_ADDRESS LastThrownObjectHandle;
	CLRDATA_ADDRESS NextThread;
};

enum class CodeHeapType {
	Loader,
	Host,
	Unknown
};

struct HandleData {
	CLRDATA_ADDRESS AppDomain;
	CLRDATA_ADDRESS Handle;
	CLRDATA_ADDRESS Secondary;
	DWORD Type;
	DWORD StrongReference;

	// For RefCounted Handles
	DWORD RefCount;
	DWORD JupiterRefCount;
	DWORD IsPegged;
};

struct AppDomainStoreData {
	CLRDATA_ADDRESS SharedDomain;
	CLRDATA_ADDRESS SystemDomain;
	int AppDomainCount;
};

struct SyncBlockData {
	CLRDATA_ADDRESS Object;
	DWORD Free;
	CLRDATA_ADDRESS Address;
	DWORD COMFlags;
	DWORD MonitorHeld;
	DWORD Recursion;
	CLRDATA_ADDRESS HoldingThread;
	DWORD AdditionalThreadCount;
	CLRDATA_ADDRESS AppDomain;
	DWORD TotalSyncBlockCount;
};

struct MethodTableCollectibleData {
	CLRDATA_ADDRESS LoaderAllocatorObjectHandle;
	DWORD Collectible;
};

struct CodeHeaderData {
	CLRDATA_ADDRESS GCInfo;
	DWORD JITType;
	CLRDATA_ADDRESS MethodDesc;
	CLRDATA_ADDRESS MethodStart;
	DWORD MethodSize;
	CLRDATA_ADDRESS ColdRegionStart;
	DWORD ColdRegionSize;
	DWORD HotRegionSize;
};

struct MethodTableData {
	DWORD IsFree; // everything else is NULL if this is true.
	CLRDATA_ADDRESS Module;
	CLRDATA_ADDRESS EEClass;
	CLRDATA_ADDRESS ParentMethodTable;
	WORD NumInterfaces;
	WORD NumMethods;
	WORD NumVtableSlots;
	WORD NumVirtuals;
	DWORD BaseSize;
	DWORD ComponentSize;
	DWORD Token;
	DWORD AttrClass;
	DWORD Shared; // flags & enum_flag_DomainNeutral
	DWORD Dynamic;
	DWORD ContainsPointers;
};

struct CCWData {
	CLRDATA_ADDRESS OuterIUnknown;
	CLRDATA_ADDRESS ManagedObject;
	CLRDATA_ADDRESS Handle;
	CLRDATA_ADDRESS CCWAddress;

	int RefCount;
	int InterfaceCount;
	DWORD IsNeutered;

	int JupiterRefCount;
	DWORD IsPegged;
	DWORD IsGlobalPegged;
	DWORD HasStrongRef;
	DWORD IsExtendsCOMObject;
	DWORD HasWeakReference;
	DWORD IsAggregated;
};

struct WorkRequestData {
	CLRDATA_ADDRESS Function;
	CLRDATA_ADDRESS Context;
	CLRDATA_ADDRESS NextWorkRequest;
};

struct V4FieldInfo {
	short NumInstanceFields;
	short NumStaticFields;
	short NumThreadStaticFields;
	CLRDATA_ADDRESS FirstFieldAddress; // If non-null, you can retrieve more
	short ContextStaticOffset;
	short ContextStaticsSize;
};

struct RCWData {
	CLRDATA_ADDRESS IdentityPointer;
	CLRDATA_ADDRESS IUnknownPointer;
	CLRDATA_ADDRESS ManagedObject;
	CLRDATA_ADDRESS JupiterObject;
	CLRDATA_ADDRESS VTablePointer;
	CLRDATA_ADDRESS CreatorThread;
	CLRDATA_ADDRESS CTXCookie;

	int RefCount;
	int InterfaceCount;

	DWORD IsJupiterObject;
	DWORD SupportsIInspectable;
	DWORD IsAggregated;
	DWORD IsContained;
	DWORD IsFreeThreaded;
	DWORD IsDisconnected;
};

struct V45ObjectData {
	CLRDATA_ADDRESS MethodTable;
	DWORD ObjectType;
	ULONGLONG Size;
	CLRDATA_ADDRESS ElementTypeHandle;
	DWORD ElementType;
	DWORD Rank;
	ULONGLONG NumComponents;
	ULONGLONG ComponentSize;
	CLRDATA_ADDRESS ArrayDataPointer;
	CLRDATA_ADDRESS ArrayBoundsPointer;
	CLRDATA_ADDRESS ArrayLowerBoundsPointer;
	CLRDATA_ADDRESS RCW;
	CLRDATA_ADDRESS CCW;
};

struct ThreadPoolData {
	int CpuUtilization;
	int NumIdleWorkerThreads;
	int NumWorkingWorkerThreads;
	int NumRetiredWorkerThreads;
	int MinLimitTotalWorkerThreads;
	int MaxLimitTotalWorkerThreads;

	CLRDATA_ADDRESS FirstUnmanagedWorkRequest;

	CLRDATA_ADDRESS HillClimbingLog;
	int HillClimbingLogFirstIndex;
	int HillClimbingLogSize;

	int NumTimers;

	int NumCPThreads;
	int NumFreeCPThreads;
	int MaxFreeCPThreads;
	int NumRetiredCPThreads;
	int MaxLimitTotalCPThreads;
	int CurrentLimitTotalCPThreads;
	int MinLimitTotalCPThreads;

	CLRDATA_ADDRESS AsyncTimerCallbackCompletionFPtr;
};

struct SegmentData {
	CLRDATA_ADDRESS Address;
	CLRDATA_ADDRESS Allocated;
	CLRDATA_ADDRESS Committed;
	CLRDATA_ADDRESS Reserved;
	CLRDATA_ADDRESS Used;
	CLRDATA_ADDRESS Start;
	CLRDATA_ADDRESS Next;
	CLRDATA_ADDRESS Heap;
	CLRDATA_ADDRESS HighAllocMark;
	SIZE_T Flags;
	CLRDATA_ADDRESS BackgroundAllocated;
};

struct CommonMethodTables {
	ULONGLONG ArrayMethodTable;
	ULONGLONG StringMethodTable;
	ULONGLONG ObjectMethodTable;
	ULONGLONG ExceptionMethodTable;
	ULONGLONG FreeMethodTable;
};

struct AppDomainInfo : AppDomainData {
	CString Name;
};

struct AssemblyInfo : AssemblyData {
	CString Name;
};

struct MethodTableInfo : MethodTableData {
	DWORD Index;
	CString Name;
};

struct ThreadInfo : ThreadData {
	ULONGLONG StackLow;
	ULONGLONG StackHigh;
	ULONGLONG StackCurrent;
};

