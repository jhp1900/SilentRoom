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
		DUIMSG_HANDLER(kAM_IPSetupMsg, OnIpSetupMsg)
		DUIMSG_HANDLER(kAM_WebRetMsg, OnWebRetMsg)
		DUIMSG_HANDLER(kAM_BroadcastTeacher, OnPlayStream)
		DUIMSG_HANDLER(kAM_BeginKeepaLive, OnBeginKeepaLive)
		DUIMSG_HANDLER(kAM_PlayErrorToEnd, OnPlayErrorToEndMsg)
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
	LRESULT OnIpSetupMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnWebRetMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnPlayStream(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	LRESULT OnBeginKeepaLive(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);		// 启动心跳包
	LRESULT OnPlayErrorToEndMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);		// 启动心跳包

	void AddTray();			// 添加托盘
	void Animation();
	void StartRpcThread();
	bool PlayStream(const string &stream_ip, const string &msg_str);	// 播放stream_ip地址的视频流
	void StopStream();

private:
	NOTIFYICONDATA tray_data_;
	string local_ip_;
	bool show_wnd_;

	std::shared_ptr<MenuWnd> tray_menu_;
	std::shared_ptr<VideoWnd> Video_wnd_;
	std::shared_ptr<RpcServer> rpc_server_;
	std::shared_ptr<std::thread> rpc_listen_thread_;
	std::shared_ptr<WebStudentClient> web_client_;

	int alpha_;
	HWND play_hwnd_;
	std::string speaker_;		// 发言者，但没有人在发言是给字符串为 空
	StudentData last_sutdentdata_;
};

