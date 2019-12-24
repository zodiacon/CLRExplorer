#pragma once

struct FormatHelper {
	template<typename T>
	static CString ToHex(T value) {
		CString text;
		text.Format(L"0x%X", value);
		return text;
	}

	template<typename T>
	static CString ToDec(T value) {
		CString text;
		text.Format(L"%u", value);
		return text;
	}

	template<>
	static CString ToHex(PVOID value) {
		CString text;
		text.Format(L"0x%p", value);
		return text;
	}
};

