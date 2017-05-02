#ifndef __CALL_IVGA_HOOK_H__
#define __CALL_IVGA_HOOK_H__

#include <windows.h>
#include <functional>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#ifdef __IVGA_CALL_API__
#define IVGA_HOOK_API extern "C" __declspec(dllexport)
#else
#define IVGA_HOOK_API extern "C" __declspec(dllexport)
#endif

typedef std::function<BOOL(int, WPARAM, LPARAM)> IvgaFunc;

enum IVGA_HOOK_TYPE
{
	WndProc_Hook = 0,
	Keyboard_Hook,
	Mouse_Hook
};

IVGA_HOOK_API bool SetKeyboardHook(IvgaFunc& cb_func, IVGA_HOOK_TYPE hook_type = Keyboard_Hook);
IVGA_HOOK_API bool UnKeyboardHook();
IVGA_HOOK_API void InterceptMsg(bool intercept = true);

#endif // !__CALL_IVGA_HOOK_H__
