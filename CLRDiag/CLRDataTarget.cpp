#include "pch.h"
#include "CLRDataTarget.h"
#include "DataTarget.h"

HRESULT CCLRDataTarget::Init(DataTarget* dt) {
	m_Target = dt;
	m_hProcess = ::OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, dt->GetProcessId());
	if (!m_hProcess)
		return HRESULT_FROM_WIN32(::GetLastError());

	return S_OK;
}

void CCLRDataTarget::FinalRelease() {
	if (m_hProcess)
		::CloseHandle(m_hProcess);
}

HRESULT __stdcall CCLRDataTarget::GetMachineType(ULONG32* machineType) {
	*machineType = sizeof(PVOID) == 8 ? IMAGE_FILE_MACHINE_AMD64 : IMAGE_FILE_MACHINE_I386;
	return S_OK;
}

HRESULT __stdcall CCLRDataTarget::GetPointerSize(ULONG32* pointerSize) {
	*pointerSize = sizeof(PVOID);

	return S_OK;
}

HRESULT __stdcall CCLRDataTarget::GetImageBase(LPCWSTR imagePath, CLRDATA_ADDRESS* baseAddress) {
	auto hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_Target->GetProcessId());
	MODULEENTRY32 me;
	me.dwSize = sizeof(me);
	Module32First(hSnapshot, &me);
	do {
		auto filename = ::wcsrchr(me.szExePath, L'\\') + 1;
		if (_wcsicmp(filename, imagePath) == 0) {
			*baseAddress = (CLRDATA_ADDRESS)me.modBaseAddr;
			break;
		}
	} while (::Module32Next(hSnapshot, &me));
	::CloseHandle(hSnapshot);
	return S_OK;
}

HRESULT __stdcall CCLRDataTarget::ReadVirtual(CLRDATA_ADDRESS address, BYTE* buffer, ULONG32 bytesRequested, ULONG32* bytesRead) {
	SIZE_T read;
	auto ok = ::ReadProcessMemory(m_hProcess, (void*)address, buffer, bytesRequested, &read);
	*bytesRead = ok ? (ULONG32)read : 0;

	return ok ? S_OK : HRESULT_FROM_WIN32(::GetLastError());
}

HRESULT __stdcall CCLRDataTarget::WriteVirtual(CLRDATA_ADDRESS address, BYTE* buffer, ULONG32 bytesRequested, ULONG32* bytesWritten) {
	SIZE_T written;
	auto ok = ::WriteProcessMemory(m_hProcess, (void*)address, buffer, bytesRequested, &written);
	if (ok)
		*bytesWritten = (ULONG32)written;
	return ok ? S_OK : HRESULT_FROM_WIN32(::GetLastError());
}

HRESULT __stdcall CCLRDataTarget::GetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS* value) {
	return E_NOTIMPL;
}

HRESULT __stdcall CCLRDataTarget::SetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS value) {
	return E_NOTIMPL;
}

HRESULT __stdcall CCLRDataTarget::GetCurrentThreadID(ULONG32* threadID) {
	return E_NOTIMPL;
}

HRESULT __stdcall CCLRDataTarget::GetThreadContext(ULONG32 threadID, ULONG32 contextFlags, ULONG32 contextSize, BYTE* context) {
	return E_NOTIMPL;
}

HRESULT __stdcall CCLRDataTarget::SetThreadContext(ULONG32 threadID, ULONG32 contextSize, BYTE* context) {
	return E_NOTIMPL;
}

HRESULT __stdcall CCLRDataTarget::Request(ULONG32 reqCode, ULONG32 inBufferSize, BYTE* inBuffer, ULONG32 outBufferSize, BYTE* outBuffer) {
	return E_NOTIMPL;
}
