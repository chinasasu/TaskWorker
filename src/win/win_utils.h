/* -------------------------------------------------------------------------
//     filename :	win_utils.h
//     author   :	cjzhou@tencent.cn
//     created  :	2016-4-13 20:25
//     purpose  :
//
// -----------------------------------------------------------------------*/
#ifndef __WIN_UTILS_H__
#define __WIN_UTILS_H__

#include <windows.h>

typedef int(__cdecl *WinProcExceptionFilter)(EXCEPTION_POINTERS* info);

HMODULE GetModuleFromAddress(void* address);

int CallExceptionFilter(EXCEPTION_POINTERS* info);

//void InitializeWindowClass(
//	const char16* class_name,
//	WNDPROC window_proc,
//	UINT style,
//	int class_extra,
//	int window_extra,
//	HCURSOR cursor,
//	HBRUSH background,
//	const char16* menu_name,
//	HICON large_icon,
//	HICON small_icon,
//	WNDCLASSEX* class_out);

template <WNDPROC proc>
LRESULT CALLBACK WrappedWindowProc(HWND hwnd, UINT message,
	WPARAM wparam, LPARAM lparam) {
	LRESULT rv = 0;
	__try {
		rv = proc(hwnd, message, wparam, lparam);
	}
	__except (CallExceptionFilter(GetExceptionInformation())) {
	}
	return rv;
}


void SetThreadName(DWORD dwThreadID, const char* threadName);

#endif /* __WIN_UTILS_H__ */