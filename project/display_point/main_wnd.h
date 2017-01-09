#pragma once
#include "stdafx.h"
#include "msg_head.h"
#include "menu_wnd.h"
#include <memory>

class MainWnd : public WindowImplBase
{
public:
	MainWnd();
	~MainWnd();

	DECLARE_DUIWND_INFO(_T("MainWnd"), CS_DBLCLKS, _T("display_main_wnd.xml"))

	BEGIN_DUIMSG_MAP(MainWnd)
		DUIMSG_HANDLER(kAM_TrayCallbackMsg, OnTray)
		DUIMSG_HANDLER(kAM_TrayMenuMsg, OnTrayMenuMsg)
	END_DUIMSG_MAP()

public:
	virtual void InitWindow() override;

private:
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override;
	LRESULT OnTray(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnTrayMenuMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);

	void AddTray();			// 添加托盘
	void GetLocalIP();		// 获取本机IP

private:
	NOTIFYICONDATA tray_data_;
	string ip_info_;
	bool show_wnd_;
	std::shared_ptr<MenuWnd> tray_menu_;
};
