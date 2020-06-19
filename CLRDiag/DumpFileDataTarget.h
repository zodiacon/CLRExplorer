#pragma once

#include "DataTarget.h"

class CDumpCLRDataTarget;

class DumpFileDataTarget final : public DataTarget {
public:
	DumpFileDataTarget(PCWSTR path);

	DWORD GetProcessId() const override;
	CString GetProcessPathName() override;
	FILETIME GetProcessStartTime() override;

protected:
	HRESULT Init() override;

private:
	bool FindModule(PCWSTR name, CString& path);

private:
	CString _path;
	CDumpCLRDataTarget* _dumpTarget;
};

