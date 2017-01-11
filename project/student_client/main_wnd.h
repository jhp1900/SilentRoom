#pragma once
#include "stdafx.h"
#include "msg_head.h"
#include <memory>
#include "rpc_client.h"

class MainWnd : public WindowImplBase
{
public:
	MainWnd();
	~MainWnd();

	DECLARE_DUIWND_INFO(_T("MainWnd"), CS_DBLCLKS, _T("stuc_main_wnd.xml"))

	BEGIN_DUIMSG_MAP(MainWnd)
		//DUIMSG_HANDLER(kAM_InitOtherWndMsg, OnInitOtherWndMsg)
		//DUIMSG_HANDLER(kAM_ResetIPInfo, OnResetIPInfoMsg)
		//DUIMSG_HANDLER(kAM_DPISetMsg, OnDPISetMsg)
		DUIMSG_HANDLER(kAM_TrayCallbackMsg, OnTray)
	END_DUIMSG_MAP()

	BEGIN_DUINOTIFY_MAP(MainWnd)
		DUINOTIFY_TYPE_HANDLER(DUI_MSGTYPE_CLICK, OnClickBtn)
	END_DUINOTIFY_MAP()

public:
	virtual void InitWindow() override;

private:
	void OnClickBtn(TNotifyUI &msg, bool &handled);		// 点击事件过滤器

	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override;
	LRESULT OnTray(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);

	bool Login();			// 响应登录事件
	void LoginAnimation();	// 登录动效
	void Speak();			// 响应发言事件
	void AddTray();			// 添加托盘
	void GetLocalIP();		// 获取本机IP
	void AutoGetIp();		// 设置为自动获取IP

private:
	NOTIFYICONDATA tray_data_;
	string ip_info_;
	std::shared_ptr<RpcClient> rpc_client_;
};
