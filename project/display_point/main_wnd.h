#pragma once
#include "stdafx.h"
#include "..\utils\msg_head.h"
#include "menu_wnd.h"
#include <memory>
#include "native_control_ui.h"
#include "rpc_server.h"
#include <thread>
#include "..\utils\web_student_client.h"

class VideoWnd :public WindowImplBase
{
public:
	VideoWnd() {}
	~VideoWnd() {}

	HWND Init(HWND pa_hwnd);
	void SetWndShow(bool show);

	DECLARE_DUIWND_INFO(_T("VideoWnd"), CS_DBLCLKS, _T("video_wnd.xml"))
};

class MainWnd : public WindowImplBase
{
public:
	MainWnd();
	~MainWnd();

	DECLARE_DUIWND_INFO(_T("MainWnd"), CS_DBLCLKS, _T("display_main_wnd.xml"))

	BEGIN_DUIMSG_MAP(MainWnd)
		DUIMSG_HANDLER(kAM_TrayCallbackMsg, OnTray)
		DUIMSG_HANDLER(kAM_TrayMenuMsg, OnTrayMenuMsg)
		DUIMSG_HANDLER(WM_TIMER, OnTimer)
		DUIMSG_HANDLER(kAM_RpcHandupMsg, OnRpcHandupMsg)
	END_DUIMSG_MAP()

	BEGIN_DUICONTROL_CREATE(Manager)
		DUICONTROL_CREATE(_T("Panel"), NativeControlUI)
	END_DUICONTROL_CREATE()

public:
	virtual void InitWindow() override;

private:
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) override;
	LRESULT OnTray(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnTrayMenuMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnRpcHandupMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);

	void AddTray();			// 添加托盘
	void GetLocalIP();		// 获取本机IP
	void Animation();
	void StartRpcThread();
	void StartCurlClient();
	bool PlayStream(string url, LPCTSTR point_text);	// 播放url指定视频流
	void StopStream();

private:
	NOTIFYICONDATA tray_data_;
	string ip_info_;
	bool show_wnd_;

	std::shared_ptr<MenuWnd> tray_menu_;
	std::shared_ptr<VideoWnd> Video_wnd_;
	std::shared_ptr<RpcServer> rpc_server_;
	std::shared_ptr<std::thread> rpc_listen_thread_;
	std::shared_ptr<WebStudentClient> web_client_;

	int alpha_;
	HWND play_hwnd_;
};

