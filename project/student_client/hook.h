#ifndef __HOOK_H__
#define __HOOK_H__

#include "call_ivga_hook.h"
#pragma comment(lib, "ivga_hook.lib")

enum FastKey {
	FastKey
};

class SCHook {
public:
	SCHook() : viewer_(nullptr) {
		process_id_ = GetCurrentProcessId();
		enable_shorcut_key_ = false;
	}

	void SetViewer(HWND hwnd) {
		viewer_ = hwnd;
	}

	bool SetSChook() {
		IvgaFunc callback_func = std::bind(&SCHook::HookCallBack
			, this
			, std::placeholders::_1
			, std::placeholders::_2
			, std::placeholders::_3);

		return SetKeyboardHook(callback_func, Keyboard_Hook);
	}

	bool UnSCHook() {
		UnKeyboardHook();
	}

	void SetSCInterceptMsg(bool intercept = true, bool keyboard_control = false, bool keyboard_enable_shortcut_key = false) {
		if (!keyboard_control) {
			InterceptMsg(intercept);
			enable_shorcut_key_ = intercept;
		}
		else {
			InterceptMsg(true);
			keyboard_enable_shortcut_key = keyboard_enable_shortcut_key;
		}
	}

	BOOL HookCallBack(int code, WPARAM wparam, LPARAM lparam) {
		DWORD keyboard_msg = wparam;
		KBDLLHOOKSTRUCT* keyboard_info = (KBDLLHOOKSTRUCT*)lparam;
		DWORD process_id;
		HWND hwnd = GetForegroundWindow();
		GetWindowThreadProcessId(hwnd, &process_id);

		if (process_id_ != process_id) {
			return true;
		}
		if (!enable_shorcut_key_ && keyboard_info->vkCode != 0x23)
			return true;
		HWND focus_hwnd = GetFocus();
		wchar_t class_name_buf[128];
		GetClassName(focus_hwnd, class_name_buf, 128);
		if (keyboard_msg == WM_KEYUP) {
			if(1 == HIBYTE(GetKeyState(VK_LCONTROL)) >> 7)

				if (1 == HIBYTE(GetKeyState(VK_LCONTROL)) >> 7
					&& (keyboard_info->vkCode == 0x5a
						|| keyboard_info->vkCode == 0x58)) {
					if (viewer_)
						::PostMessage(viewer_, FastKey, keyboard_info->vkCode, 0);
				}
		}
	}

private:
	DWORD process_id_;
	HWND viewer_;
	bool enable_shorcut_key_;
};

#endif // !__HOOK_H__