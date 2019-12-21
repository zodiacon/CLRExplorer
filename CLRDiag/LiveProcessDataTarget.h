#pragma once

#include "DataTarget.h"

class LiveProcessDataTarget final : public DataTarget {
public:
	LiveProcessDataTarget(DWORD pid);

	// Inherited via DataTarget
	DWORD GetProcessId() const override;
	bool Suspend() override;
	bool Resume() override;

protected:
	HRESULT Init() override;

private:
	bool FindModule(PCWSTR name, CString& path) const;

private:
	DWORD _pid;
};

