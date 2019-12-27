#pragma once

#include <string>

struct FormatHelper {
	template<typename T>
	static CString ToHex(T value) {
		CString text;
		text.Format(L"0x%X", value);
		return text;
	}

	template<typename T>
	static CString ToDec(T value, int digits = 0) {
		CString text;
		if (digits == 0)
			text.Format(L"%u", value);
		else
			text.Format((L"%" + std::to_wstring(digits) + L"u").c_str(), value);
		return text;
	}

	template<>
	static CString ToHex(PVOID value) {
		CString text;
		text.Format(L"0x%p", value);
		return text;
	}
};

