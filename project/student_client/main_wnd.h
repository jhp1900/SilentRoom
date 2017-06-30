#pragma once
#include "stdafx.h"
#include "..\utils\msg_head.h"
#include <memory>
#include "menu_wnd.h"
#include "rpc_client.h"
#include "..\utils\json_operate.h"
#include "..\utils\web_student_client.h"
#include "hook.h"
#include <wlanapi.h>

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
		DUIMSG_HANDLER(kAM_IPSetupMsg, OnIpSetupMsg)
		DUIMSG_HANDLER(kAM_ChoiceNICMsg, OnChoiceNICMsg)
		DUIMSG_HANDLER(WM_RBUTTONDOWN, OnMouseMoveWnd)
		DUIMSG_HANDLER(WM_RBUTTONUP, OnMouseMoveWnd)
		//DUIMSG_HANDLER(WM_MOUSELEAVE, OnMouseMoveWnd)
		DUIMSG_HANDLER(WM_MOUSELEAVE, OnMouseLeaveWnd)
		DUIMSG_HANDLER(WM_MOUSEMOVE, OnMouseMoveWnd)
		DUIMSG_HANDLER(WM_MOUSEHOVER, OnMouseHover)
		DUIMSG_HANDLER(WM_TIMER, OnMouseTimer)
		DUIMSG_HANDLER(WM_NCLBUTTONDBLCLK, OnMouseMoveWnd)
		//DUIMSG_HANDLER(FastKey, OnHotKey);
		DUIMSG_HANDLER(WM_HOTKEY, OnHotKey)
	    //DUIMSG_HANDLER( WM_LBUTTONDOWN, OnMyLButtonDown)
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
	LRESULT OnIpSetupMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnChoiceNICMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnMouseMoveWnd(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnNcLButDbClk(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnHotKey(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnMouseLeaveWnd(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnMouseTimer(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	//LRESULT OnMyLButtonDown(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);

	bool WebClientInit();
	bool Login();			// 响应登录事件
	void Logout();			// 登出
	void LoginAnimation();	// 登录动效
	void Speak();			// 响应发言事件
	void StopSpeak();
	bool HandUp();
	//void SetupWnd();
	void AddTray();			// 添加托盘
	void OnCloseMsg();
	bool InitNativeWifi();
	bool ConnectWifi(GUID guid);
	bool OnConnBtn();
	bool OnChioceIp(LPCTSTR ip);

private:
	NOTIFYICONDATA tray_data_;
	StudentData stu_info_;
	POINT old_point_;
	POINT cursor_point_;
	POINT mouse_move_old_position_;
	bool move_now_;
	std::string local_ip_;
	TRACKMOUSEEVENT track_mouse_event_;

	std::shared_ptr<MenuWnd> tray_menu_;
	std::shared_ptr<RpcClient> rpc_client_;
	std::shared_ptr<JsonOperate> json_operate_;
	std::shared_ptr<WebStudentClient> web_client_;

	bool login_succeed_;
	bool timer_mouse_leave_;
	bool speak_button_ont_the_edge_;
	HWND login_hwnd_;
	bool have_server_ip_;

	int wnd_w_, wnd_h_;		// 窗体的宽高
	int ep_x_, ep_y_;		// 登录动效终点位置
	int hide_w_;			// 隐藏状态的窗体宽度

	HANDLE hClient_;
	std::vector<std::pair<GUID, std::wstring>> wlan_nic_;
};
