#pragma once

class DataTarget;

struct TabFactory {
	static HWND CreateTab(DataTarget* dt, DWORD_PTR type, HWND hParent, int& image);
};

