#pragma once
#include <WinUser.h>

enum MyMsg
{
	kAM_InitOtherWndMsg = WM_APP + 10,
	kAM_TrayCallbackMsg,		// 托盘图标回掉信息
	kAM_TrayMenuMsg,			// 托盘菜单消息
	kAM_WebRetMsg,				// web server 返回消息
	kAM_RpcHandupMsg,			// RPC 举手消息
	kAM_IPSetupMsg,				// 设置界面， IP设置完成后发送的信息
	kAM_BroadcastTeacher		// 教师广播消息
};