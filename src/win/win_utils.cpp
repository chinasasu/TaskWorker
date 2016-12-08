#include "win/win_utils.h"
#include <psapi.h>

WinProcExceptionFilter s_exception_filter = NULL;

int CallExceptionFilter(EXCEPTION_POINTERS* info)
{
	return s_exception_filter ? s_exception_filter(info) :
		EXCEPTION_CONTINUE_SEARCH;
}

HMODULE GetModuleFromAddress(void* address)
{
	HMODULE instance = NULL;
	if (!::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		static_cast<char*>(address),
		&instance)) {
		//NOTREACHED();
	}
	return instance;

}

//#define SLEEP_TIME	10
const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(DWORD dwThreadID, const char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

//void InitializeWindowClass(
//	const wchar_t* class_name,
//	WNDPROC window_proc,
//	UINT style,
//	int class_extra,
//	int window_extra,
//	HCURSOR cursor,
//	HBRUSH background,
//	const char16* menu_name,
//	HICON large_icon,
//	HICON small_icon,
//	WNDCLASSEX* class_out) {
//	class_out->cbSize = sizeof(WNDCLASSEX);
//	class_out->style = style;
//	class_out->lpfnWndProc = window_proc;
//	class_out->cbClsExtra = class_extra;
//	class_out->cbWndExtra = window_extra;
//	class_out->hInstance = GetModuleFromAddress(window_proc);
//	class_out->hIcon = large_icon;
//	class_out->hCursor = cursor;
//	class_out->hbrBackground = background;
//	class_out->lpszMenuName = menu_name;
//	class_out->lpszClassName = class_name;
//	class_out->hIconSm = small_icon;
//
//	// Check if |window_proc| is valid.
//	DCHECK(class_out->hInstance != NULL);
//}