#pragma once
#include <WinUser.h>

enum MyMsg
{
	kAM_InitOtherWndMsg = WM_APP + 10,
	kAM_TrayCallbackMsg,		// ����ͼ��ص���Ϣ
	kAM_TrayMenuMsg,			// ���̲˵���Ϣ
	kAM_WebRetMsg,				// web server ������Ϣ
};