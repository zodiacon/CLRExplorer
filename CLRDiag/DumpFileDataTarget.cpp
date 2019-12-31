#include "pch.h"
#include "DumpFileDataTarget.h"
#include "DumpCLRDataTarget.h"

DumpFileDataTarget::DumpFileDataTarget(PCWSTR path) : _path(path) {
}

DWORD DumpFileDataTarget::GetProcessId() const {
	return _dumpTarget->GetProcessId();
}

CString DumpFileDataTarget::GetProcessPathName() {
	return _dumpTarget->GetProcessImagePath();
}

FILETIME DumpFileDataTarget::GetProcessStartTime() {
	return _dumpTarget->GetProcessCreateTime();
}

HRESULT DumpFileDataTarget::Init() {
	CComObject<CDumpCLRDataTarget>* target;
	target->CreateInstance(&target);
	auto hr = target->Init(_path);
	_clrTarget = target;

	if (FAILED(hr))
		return hr;

	_dumpTarget = target;

	CString path, dacFile;
	if (FindModule(L"clr.dll", path))
		dacFile = L"mscordacwks.dll";
	else if (FindModule(L"coreclr.dll", path))
		dacFile = L"mscordaccore.dll";

	if (path.IsEmpty())
		return E_UNEXPECTED;

	path = path.Left(path.ReverseFind(L'\\') + 1) + dacFile;

	auto hDac = ::LoadLibrary(path);
	if (!hDac)
		return HRESULT_FROM_WIN32(::GetLastError());

	auto pCLRCreateData = (PFN_CLRDataCreateInstance)::GetProcAddress(hDac, "CLRDataCreateInstance");
	if (!pCLRCreateData)
		return HRESULT_FROM_WIN32(::GetLastError());

	CComPtr<ISOSDacInterface> spSos;
	hr = pCLRCreateData(__uuidof(ISOSDacInterface), target, reinterpret_cast<void**>(&spSos));
	if (FAILED(hr))
		return hr;

	_spSos = spSos;
	return hr;
}

bool DumpFileDataTarget::FindModule(PCWSTR name, CString& path) {
	return _dumpTarget->FindModule(name, path);
}
