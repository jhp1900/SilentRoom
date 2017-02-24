#pragma once
#include "stdafx.h"
#include "msg_head.h"
#include <memory>
#include "menu_wnd.h"
#include "rpc_client.h"
#include "json_operate.h"
#include "web_student_client.h"

class MainWnd : public WindowImplBase
{
public:
	MainWnd();
	~MainWnd();

	DECLARE_DUIWND_INFO(_T("MainWnd"), CS_DBLCLKS, _T("stuc_main_wnd.xml"))

	BEGIN_DUIMSG_MAP(MainWnd)
		DUIMSG_HANDLER(kAM_TrayCallbackMsg, OnTray)
		DUIMSG_HANDLER(kAM_TrayMenuMsg, OnTrayMenuMsg)
		DUIMSG_HANDLER(kAM_WebRetMsg, OnWebRetMsg)
		DUIMSG_HANDLER(WM_RBUTTONDOWN, OnMouseMoveWnd)
		DUIMSG_HANDLER(WM_RBUTTONUP, OnMouseMoveWnd)
		DUIMSG_HANDLER(WM_MOUSELEAVE, OnMouseMoveWnd)
		DUIMSG_HANDLER(WM_MOUSEMOVE, OnMouseMoveWnd)
		DUIMSG_HANDLER(WM_NCLBUTTONDBLCLK, OnMouseMoveWnd)
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
	LRESULT OnTrayMenuMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnWebRetMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnMouseMoveWnd(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnNcLButDbClk(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);

	void WebClientInit();
	bool Login();			// 响应登录事件
	void LoginAnimation();	// 登录动效
	void Speak();			// 响应发言事件
	void StopSpeak();
	bool HandUp();
	//void SetupWnd();
	void AddTray();			// 添加托盘
	void GetLocalIP();		// 获取本机IP
	void AutoGetIp();		// 设置为自动获取IP

private:
	NOTIFYICONDATA tray_data_;
	StudentData stu_info_;
	POINT old_point_;
	POINT cursor_point_;
	bool move_now_;
	std::string local_ip_;
	TRACKMOUSEEVENT track_mouse_event_;

	std::shared_ptr<MenuWnd> tray_menu_;
	std::shared_ptr<RpcClient> rpc_client_;
	std::shared_ptr<JsonOperate> json_operate_;
	std::shared_ptr<WebStudentClient> web_client_;

	HWND login_hwnd_;
	bool have_server_ip_;
};
