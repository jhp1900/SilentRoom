#pragma once
#include <WinUser.h>

enum MyMsg
{
	kAM_InitOtherWndMsg = WM_APP + 10,
	kAM_TrayCallbackMsg,		// ����ͼ��ص���Ϣ
	kAM_TrayMenuMsg,			// ���̲˵���Ϣ
	kAM_WebRetMsg,				// web server ������Ϣ
	kAM_RpcHandupMsg,			// RPC ������Ϣ
	kAM_IPSetupMsg,				// ���ý��棬 IP������ɺ��͵���Ϣ
	kAM_BroadcastTeacher		// ��ʦ�㲥��Ϣ
};