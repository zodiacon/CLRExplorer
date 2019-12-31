#include "pch.h"
#include "DumpCLRDataTarget.h"

#pragma comment(lib, "dbgeng")

#define HR(x) { auto _hr = (x); if(FAILED(_hr)) return _hr; }

HRESULT CDumpCLRDataTarget::Init(PCWSTR dumpFilePath) {
	HR(::DebugCreate(__uuidof(IDebugClient6), reinterpret_cast<void**>(&m_spDebug)));
	m_spControl = m_spDebug;
	ATLASSERT(m_spControl);
	m_spAdvanced = m_spDebug;
	ATLASSERT(m_spAdvanced);
	m_spDataSpaces = m_spDebug;
	ATLASSERT(m_spDataSpaces);
	m_spSysObjects = m_spDebug;
	ATLASSERT(m_spSysObjects);

	HR(m_spDebug->OpenDumpFileWide(dumpFilePath, 0));
	ULONG64 handle;
	ULONG type;
	HR(m_spDebug->GetDumpFile(0, nullptr, 0, nullptr, &handle, &type));
	m_hFile = (HANDLE)handle;
	HR(m_spControl->WaitForEvent(0, INFINITE));

	return S_OK;
}

HRESULT __stdcall CDumpCLRDataTarget::GetMachineType(ULONG32* machineType) {
	//MINIDUMP_SYSTEM_INFO sysInfo;
	//DEBUG_READ_USER_MINIDUMP_STREAM input = { 0 };
	//input.StreamType = SystemInfoStream;
	//input.Buffer = &sysInfo;
	//input.BufferSize = sizeof(sysInfo);
	//ULONG outSize;
	//HR(m_spAdvanced->Request(DEBUG_REQUEST_READ_USER_MINIDUMP_STREAM, &input, sizeof(input), nullptr, 0, &outSize));

	//*machineType = sysInfo.ProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? IMAGE_FILE_MACHINE_AMD64 : IMAGE_FILE_MACHINE_I386;

	* machineType = sizeof(PVOID) == 8 ? IMAGE_FILE_MACHINE_AMD64 : IMAGE_FILE_MACHINE_I386;

	return S_OK;
}

HRESULT __stdcall CDumpCLRDataTarget::GetPointerSize(ULONG32* pointerSize) {
	*pointerSize = sizeof(PVOID);
	return S_OK;
}

HRESULT __stdcall CDumpCLRDataTarget::GetImageBase(LPCWSTR imagePath, CLRDATA_ADDRESS* baseAddress) {
	CString path;
	MINIDUMP_MODULE module;
	if(FindModule(imagePath, path, &module)) {
		*baseAddress = (CLRDATA_ADDRESS)module.BaseOfImage;
		return S_OK;
	}

	return E_INVALIDARG;
}

HRESULT __stdcall CDumpCLRDataTarget::ReadVirtual(CLRDATA_ADDRESS address, BYTE* buffer, ULONG32 bytesRequested, ULONG32* bytesRead) {
	return m_spDataSpaces->ReadVirtual(address, buffer, bytesRequested, (PULONG)bytesRead);
}

HRESULT __stdcall CDumpCLRDataTarget::WriteVirtual(CLRDATA_ADDRESS address, BYTE* buffer, ULONG32 bytesRequested, ULONG32* bytesWritten) {
	return m_spDataSpaces->WriteVirtual(address, buffer, bytesRequested, (PULONG)bytesWritten);
}

HRESULT __stdcall CDumpCLRDataTarget::GetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS* value) {
	return E_NOTIMPL;
}

HRESULT __stdcall CDumpCLRDataTarget::SetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS value) {
	return E_NOTIMPL;
}

HRESULT __stdcall CDumpCLRDataTarget::GetCurrentThreadID(ULONG32* threadID) {
	return E_NOTIMPL;
}

HRESULT __stdcall CDumpCLRDataTarget::GetThreadContext(ULONG32 threadID, ULONG32 contextFlags, ULONG32 contextSize, BYTE* context) {
	return E_NOTIMPL;
}

HRESULT __stdcall CDumpCLRDataTarget::SetThreadContext(ULONG32 threadID, ULONG32 contextSize, BYTE* context) {
	return E_NOTIMPL;
}

HRESULT __stdcall CDumpCLRDataTarget::Request(ULONG32 reqCode, ULONG32 inBufferSize, BYTE* inBuffer, ULONG32 outBufferSize, BYTE* outBuffer) {
	return E_NOTIMPL;
}

CString CDumpCLRDataTarget::ReadString(DWORD rva) {
	ULONG32 len;
	DWORD read;
	::SetFilePointer(m_hFile, rva, nullptr, FILE_BEGIN);
	if (!::ReadFile(m_hFile, &len, sizeof(len), &read, nullptr))
		return L"";

	auto buffer = std::make_unique<WCHAR[]>(len);
	if (!::ReadFile(m_hFile, buffer.get(), len, &read, nullptr))
		return L"";

	return CString(buffer.get(), len / 2);
}

bool CDumpCLRDataTarget::FindModule(PCWSTR name, CString& path, MINIDUMP_MODULE* pModule) {
	DEBUG_READ_USER_MINIDUMP_STREAM input = { 0 };
	input.StreamType = ModuleListStream;
	constexpr int size = 1 << 20;
	auto buffer = std::make_unique<BYTE[]>(size);
	input.Buffer = buffer.get();
	input.BufferSize = size;
	ULONG outSize;
	HR(m_spAdvanced->Request(DEBUG_REQUEST_READ_USER_MINIDUMP_STREAM, &input, sizeof(input), nullptr, 0, &outSize));
	auto modules = reinterpret_cast<MINIDUMP_MODULE_LIST*>(buffer.get());

	for (ULONG32 i = 0; i < modules->NumberOfModules; i++) {
		auto& module = modules->Modules[i];
		auto imagePath = ReadString(module.ModuleNameRva);
		auto filename = ::wcsrchr(imagePath, L'\\') + 1;
		if (_wcsicmp(filename, name) == 0) {
			if (pModule)
				*pModule = module;
			path = imagePath;
			return true;
		}
	}

	return false;
}

DWORD CDumpCLRDataTarget::GetProcessId() const {
	GetMiscInfo();
	return m_MiscInfo->ProcessId;
}

CString CDumpCLRDataTarget::GetProcessImagePath() const {
	WCHAR name[MAX_PATH];
	ULONG len;
	return m_spSysObjects->GetCurrentProcessExecutableNameWide(name, _countof(name), &len) == S_OK ? name : L"";
}

FILETIME CDumpCLRDataTarget::GetProcessCreateTime() const {
	GetMiscInfo();
	FILETIME ft;
	time_t t = m_MiscInfo->ProcessCreateTime;
	ULONG64 ll = Int32x32To64(t, 10000000) + 116444736000000000;
	ft.dwLowDateTime = (DWORD)ll;
	ft.dwHighDateTime = ll >> 32;

	return ft;
}

bool CDumpCLRDataTarget::GetMiscInfo() const {
	if (m_MiscInfo)
		return true;

	m_MiscInfo = std::make_unique<MINIDUMP_MISC_INFO>();
	m_MiscInfo->SizeOfInfo = sizeof(MINIDUMP_MISC_INFO);
	m_MiscInfo->Flags1 = MINIDUMP_MISC1_PROCESS_ID | MINIDUMP_MISC1_PROCESS_TIMES;
	DEBUG_READ_USER_MINIDUMP_STREAM input = { 0 };
	input.StreamType = MiscInfoStream;
	input.Buffer = m_MiscInfo.get();
	input.BufferSize = sizeof(MINIDUMP_MISC_INFO);
	ULONG outSize;
	auto hr = m_spAdvanced->Request(DEBUG_REQUEST_READ_USER_MINIDUMP_STREAM, &input, sizeof(input), nullptr, 0, &outSize);

	return SUCCEEDED(hr);
}
