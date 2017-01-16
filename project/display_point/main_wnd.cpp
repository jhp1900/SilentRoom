#include "main_wnd.h"
#include "resource.h"
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <sstream>
#include <NetCon.h>
#include "application.h"
#include "vlc_tool.h"
#include "json_operate.h"
#include <atlbase.h>

#pragma comment(lib,"Iphlpapi.lib")

HWND VideoWnd::Init(HWND pa_hwnd)
{
	HWND ret_hwnd = Create(pa_hwnd, _T(""), UI_WNDSTYLE_CHILD & ~WS_VISIBLE, 0, 0, 0, 0, 0, (HMENU)(0));
	::SetWindowPos(*this, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	ShowWindow();
	return ret_hwnd;
}

void VideoWnd::SetWndShow(bool show)
{
	if(show)
		::ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	else 
		::ShowWindow(m_hWnd, SW_HIDE);
}

MainWnd::MainWnd()
	: tray_data_({0})
	, ip_info_("")
	, show_wnd_(false)
	, tray_menu_(nullptr)
	, Video_wnd_(nullptr)
	, rpc_server_(nullptr)
	, rpc_listen_thread_(nullptr)
	, web_client_(nullptr)
	, alpha_(255)
	, play_hwnd_(NULL)
{
}

MainWnd::~MainWnd()
{
	Shell_NotifyIcon(NIM_DELETE, &tray_data_);
}

void MainWnd::InitWindow()
{
	AddTray();		// 添加托盘
	GetLocalIP();	// 获取本机IP
	Animation();	// 启动动效

	// 创建流播放窗口
	Video_wnd_.reset(new VideoWnd);
	play_hwnd_ = Video_wnd_->Init(m_hWnd);

	// 初始化 RPC 服务器、启动 RPC 监听线程
	StartRpcThread();

	// 初始化、启动 ivga 广播
	if (!App::GetInstance()->GetVLCTool()->BeginBroadcast(ip_info_))
		MessageBox(m_hWnd, _T("屏幕推流失败!"), _T("Message"), MB_OK);

	// 启动 curl 客户端
	StartCurlClient();
}

LRESULT MainWnd::OnClose(UINT, WPARAM, LPARAM, BOOL & bHandled)
{
	::PostQuitMessage(0);
	return LRESULT();
}

LRESULT MainWnd::OnTray(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled)
{
	if (wparam != 1)
		return 0;
	switch (lparam)
	{
		case WM_LBUTTONDBLCLK:
			if (show_wnd_)
				::ShowWindow(m_hWnd, SW_HIDE);
			else 
				::ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
			show_wnd_ = !show_wnd_;
			break;
		case WM_RBUTTONUP:
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint);
			tray_menu_->PopupWnd(lpoint);
			break;
	}
	return LRESULT();
}

LRESULT MainWnd::OnTrayMenuMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	switch (wparam) {
		case MenuMsgClose:
			Close();
			break;
		case MenuMsgTemp1:
			PlayStream("rtsp://10.18.3.92:554/live123", _T("Stream 1"));
			break;
		case MenuMsgTemp2:
			PlayStream("rtsp://10.18.3.67:8554/live123", _T("Stream 2"));
			break;
		case MenuMsgStop:
			StopStream();
			break;
		default:
			break;
	}
	return LRESULT();
}

LRESULT MainWnd::OnTimer(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	auto point_timer = [&]() {
		KillTimer(m_hWnd, 1);
		SetTimer(m_hWnd, 2, 5, nullptr);
	};

	auto alpha_timer = [&]() {
		SetLayeredWindowAttributes(m_hWnd, 0, alpha_-=3, LWA_ALPHA);
		if (alpha_ <= 3) {
			KillTimer(m_hWnd, 2);
			alpha_ = 255;
			::ShowWindow(m_hWnd, SW_HIDE);
			SetLayeredWindowAttributes(m_hWnd, 0, alpha_, LWA_ALPHA);
		}
	};

	auto play_timer = [&]() {
		//SetTimer(m_hWnd, 4, 0, nullptr);
		m_pm.FindControl(_T("point_body"))->SetVisible(false);
		m_pm.FindControl(_T("video_body"))->SetVisible(true);
		KillTimer(m_hWnd, 3);
	};

	auto play_anima = [&]() {
		RECT point_rect = m_pm.FindControl(_T("point_body"))->GetPos();
		point_rect.bottom -= 3;
		m_pm.FindControl(_T("point_body"))->SetPos(point_rect);

		RECT video_rect = m_pm.FindControl(_T("video_body"))->GetPos();
		video_rect.top += 3;
		m_pm.FindControl(_T("video_body"))->SetPos(video_rect);

		if (point_rect.bottom <= point_rect.top)
			KillTimer(m_hWnd, 4);
	};

	switch (wparam) {
		case 1: 
			point_timer();
			break;
		case 2: 
			alpha_timer();
			break;
		case 3:
			play_timer();
			break;
		case 4:
			play_anima();
			break;
	}
	
	return LRESULT();
}

LRESULT MainWnd::OnRpcHandupMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	std::string json_str = (char*)wparam;

	JsonOperate json_operate;
	StudentData student_data = json_operate.JsonAnalysis(json_str.c_str());
	std::string url = "rtsp://" + student_data.stream_ip_ + ":554/live";
	if (PlayStream(url, CA2W(student_data.student_name_.c_str()))) {
		web_client_->SendWebMessage(json_str);
	}

	return LRESULT();
}

void MainWnd::AddTray()
{
	// 创建、初始化 托盘菜单窗体
	tray_menu_.reset(new MenuWnd(m_hWnd));
	tray_menu_->CreateWithDefaltStyle();

	// 初始化托盘结构体
	tray_data_.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	tray_data_.hWnd = this->m_hWnd;
	tray_data_.uID = 1;
	tray_data_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	tray_data_.uCallbackMessage = kAM_TrayCallbackMsg;
	tray_data_.hIcon = LoadIcon((HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDI_ICON1));
	wcscpy_s(tray_data_.szTip, L"iVGA");

	Shell_NotifyIcon(NIM_ADD, &tray_data_);		// 添加托盘
}

void MainWnd::GetLocalIP()
{
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	// 这里，stSize 既是输入量也是输出量；
	if (ERROR_BUFFER_OVERFLOW == nRel) {
		// ERROR_BUFFER_OVERFLOW 表示 传递给 GetAdaptersInfo 的内存空间不够，同时传出的 stSize 表示所需的空间大小
		delete pIpAdapterInfo;	// 释放掉，重新分配！
		pIpAdapterInfo = (PIP_ADAPTER_INFO) new BYTE[stSize];
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	// 利用传出的新空间大小值，重新填充 pIpAdapterInfo
	}
	if (ERROR_SUCCESS == nRel) {
		std::ostringstream oss;
		while (pIpAdapterInfo) {
			auto iter = &pIpAdapterInfo->IpAddressList;
			while (iter) {
				oss.str("");
				oss << iter->IpAddress.String;
				ip_info_ = oss.str();
				iter = iter->Next;
			}
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
		delete pIpAdapterInfo;	// 释放掉
	}
}

void MainWnd::Animation()
{
	::ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	// 设置透明色  		
	COLORREF cr_key = RGB(0, 0, 0);
	SetLayeredWindowAttributes(m_hWnd, cr_key, 0, LWA_COLORKEY);
	SetTimer(m_hWnd, 1, 4000, nullptr);
}

void MainWnd::StartRpcThread()
{
	auto start_rpc_listen = [&]() {
		rpc_server_.reset(new RpcServer);
		if (!rpc_server_->Initial()) 
			return;

		rpc_server_->RpcListen();
	};

	rpc_listen_thread_.reset(new std::thread(start_rpc_listen));
	rpc_listen_thread_->detach();
}

void MainWnd::StartCurlClient()
{
	// TODO ....
	// 读取配置文件，获得 服务器IP信息
	string server_ip = "http://10.18.3.67:8081";

	web_client_.reset(new WebStudentClient);
	web_client_->Initial(server_ip);
}

bool MainWnd::PlayStream(string url, LPCTSTR point_text)
{
	VLCTool *vlc = App::GetInstance()->GetVLCTool();
	vlc->DestoryPlay();
	if (!vlc->PlayStream(play_hwnd_, url)) {
		vlc->DestoryPlay();
		return false;
	}
	m_pm.FindControl(_T("point_label"))->SetText(point_text);
	::ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	show_wnd_ = true;
	SetTimer(m_hWnd, 3, 6300, nullptr);
	return true;
}

void MainWnd::StopStream()
{
	::ShowWindow(m_hWnd, SW_HIDE);
	App::GetInstance()->GetVLCTool()->DestoryPlay();
	show_wnd_ = false;
	m_pm.FindControl(_T("video_body"))->SetVisible(false);
	m_pm.FindControl(_T("point_body"))->SetVisible(true);
}
