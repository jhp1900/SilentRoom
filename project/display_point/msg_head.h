#pragma once
#include <WinUser.h>

enum MyMsg
{
	kAM_InitOtherWndMsg = WM_APP + 10,
	kAM_TrayCallbackMsg,		// 托盘图标回掉信息
	kAM_TrayMenuMsg,			// 托盘菜单消息
	kAM_RpcHandupMsg,			// RPC 举手消息
};