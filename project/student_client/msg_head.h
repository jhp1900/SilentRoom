#pragma once
#include <WinUser.h>

enum MyMsg
{
	kAM_InitOtherWndMsg = WM_APP + 10,
	kAM_TrayCallbackMsg,		// 托盘图标回掉信息
};