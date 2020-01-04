#pragma once

class DataTarget;

#include "Interfaces.h"

struct TabFactory {
	static HWND CreateTab(DataTarget* dt, DWORD_PTR type, HWND hParent, IMainFrame* frame, int& image, NodeType genericType = NodeType::None);
	static HWND CreateSpecificTab(DataTarget* dt, DWORD_PTR type, NodeType genericType, HWND hParent, IMainFrame* frame, int& image);
};

