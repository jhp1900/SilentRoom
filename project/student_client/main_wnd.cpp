#include "main_wnd.h"
#include <math.h>
#include "application.h"
#include "resource.h"
#include "vlc_tool.h"
#include <atlbase.h>
#include "..\utils\setup_wnd.h"
#include "..\utils\xml_manager.h"
#include "..\utils\utils.h"

MainWnd::MainWnd()
	: tray_data_({0})
	, local_ip_("")
	, stu_info_()
	, rpc_client_(nullptr)
	, old_point_({0})
	, cursor_point_({0})
	, move_now_(false)
	, login_hwnd_(nullptr)
	, have_server_ip_(false)
	, login_succeed_(false)
{
	stu_info_.appid_ = appid_str;
}

MainWnd::~MainWnd()
{
	UnregisterHotKey(m_hWnd, 1);
	UnregisterHotKey(m_hWnd, 2);
	UnregisterHotKey(m_hWnd, 3);
	Shell_NotifyIcon(NIM_DELETE, &tray_data_);
}

void MainWnd::InitWindow()
{
	AddTray();				// 添加托盘
	//SetAutoGetIP();			// 设置电脑为自动获取IP
	//GetLocalIP(local_ip_);	// 获取本机ip

	json_operate_.reset(new JsonOperate);
	web_client_.reset(new WebStudentClient);

	/* 鼠标事件初始化 */
	track_mouse_event_.cbSize = sizeof(TRACKMOUSEEVENT);
	track_mouse_event_.dwFlags = TME_LEAVE | TME_HOVER;
	track_mouse_event_.hwndTrack = m_hWnd;
	track_mouse_event_.dwHoverTime = 10;

	RegisterHotKey(m_hWnd, 1, MOD_CONTROL, '1');
	RegisterHotKey(m_hWnd, 2, MOD_CONTROL, '2');
	RegisterHotKey(m_hWnd, 3, MOD_CONTROL, '3');
	RegisterHotKey(m_hWnd, 4, MOD_CONTROL, 'X');
	login_hwnd_ = m_hWnd;
	WebClientInit();
}

void MainWnd::OnClickBtn(TNotifyUI & msg, bool & handled)
{
	if (msg.pSender->GetName() == _T("close_btn"))
		Close();
	else if (msg.pSender->GetName() == _T("login_btn"))
		Login();
	else if (msg.pSender->GetName() == _T("speak_btn"))
		Speak();
}

LRESULT MainWnd::OnClose(UINT, WPARAM, LPARAM, BOOL & bHandled)
{
	::PostQuitMessage(0);
	return LRESULT();
}

LRESULT MainWnd::OnTray(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	if (wparam != 1)
		return 0;
	if (lparam == WM_RBUTTONUP) {
		LPPOINT lpoint = new tagPOINT;
		::GetCursorPos(lpoint);
		tray_menu_->PopupWnd(lpoint);
	}
	return LRESULT();
}

LRESULT MainWnd::OnTrayMenuMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled)
{
	switch (wparam) {
		case MenuMsgClose:
			OnCloseMsg();
			break;
		case MenuMsgSpeak:
			Speak();
			break;
		case MenuMsgHandup:
			HandUp();
			break;
		case MenuMsgStop:
			StopSpeak();
			break;
		case MenuMsgSetup:
			SetupWnd setup_wnd(m_hWnd);
			setup_wnd.DoModal(login_hwnd_);
			break;
	}
	return LRESULT();
}

LRESULT MainWnd::OnWebRetMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	/* 解析从服务器返回的 json 数据 */
	std::string ret_data = (char*)(wparam);
	LogonInfo logon_info;
	json_operate_->JsonAnalysis(ret_data.c_str(), logon_info);

	login_hwnd_ = NULL;		// 登录成功后，"设置界面的窗口"不再以本窗口为父窗口！
	login_succeed_ = true;
	LoginAnimation();		// 登录动效

	// 初始化、启动 ivga
	if (!App::GetInstance()->GetVLCTool()->BeginBroadcast(local_ip_))
		MessageBox(m_hWnd, _T("屏幕推流失败!"), _T("Message"), MB_OK);
	stu_info_.stream_ip_ = local_ip_;

	// 初始化、启动 rpc client
	rpc_client_.reset(new RpcClient);
	rpc_client_->BindRpcServer(logon_info.group_ip.c_str(), "12322");

	return LRESULT();
}

LRESULT MainWnd::OnIpSetupMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled)
{
	WebClientInit();		// 当 IP设置完成后，重新初始化 Web 客户端
	return LRESULT();
}

LRESULT MainWnd::OnMouseMoveWnd(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	TrackMouseEvent(&track_mouse_event_);
	return LRESULT();
}

LRESULT MainWnd::OnNcLButDbClk(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	return LRESULT();
}

LRESULT MainWnd::OnHotKey(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	if (login_succeed_) {
		switch (wparam) {
		case 1: {
			OutputDebugStringA("CTRL + 1:发言 \n");
			Speak();
		}break;
		case 2: {
			OutputDebugStringA("CTRL + 2:举手 \n");
			HandUp();
		}break;
		case 3: {
			OutputDebugStringA("CTRL + 3:结束 \n");
			StopSpeak();
		}break;
		case 4: {
			OutputDebugStringA("CTRL + X:退出 \n");
			PostQuitMessage(0);
		}break;
		default:
			break;
		}
	}

	return LRESULT();
}

LRESULT MainWnd::OnMouseLeaveWnd(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	if (login_succeed_ && !speak_button_ont_the_edge_) {
		OutputDebugStringA("---mouse leave---\n");
		SetTimer(m_hWnd, 211, 4000, NULL);
		timer_mouse_leave_ = true;
	}
	return LRESULT();
}

LRESULT MainWnd::OnMouseTimer(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	int ep_x = GetSystemMetrics(SM_CXSCREEN) - 100;
	int ep_y = 200;

	auto timer_211 = [&]() {
		OutputDebugStringA("time up\n");
		SetTimer(m_hWnd, 213, 1, nullptr);
		KillTimer(this->m_hWnd, 211);
	};

	auto timer_212 = [&]() {
		RECT rect;
		GetWindowRect(m_hWnd, &rect);
		int add_x = (ep_x - rect.left) / 10;
		int add_y = (rect.top - ep_y) / 10;
		if (add_x > 1 || add_x < -1 || add_y > 1 || add_y < -1) {
			int x = rect.left + add_x;
			int y = rect.top - add_y;
			MoveWindow(m_hWnd, x, y, 100, 227, true);
		}
		else {
			KillTimer(this->m_hWnd, 212);
			MoveWindow(m_hWnd, ep_x, ep_y, 100, 227, true);
		}
	};

	auto timer_213 = [&]() {
		RECT rect;
		GetWindowRect(m_hWnd, &rect);
		int w = GetSystemMetrics(SM_CXSCREEN) - rect.left;
		if (w > 6) {
			MoveWindow(m_hWnd, rect.left + 2, rect.top, w - 2, 227, false);
		}
		else{
			MoveWindow(m_hWnd, GetSystemMetrics(SM_CXSCREEN) - 6, rect.top, 6, 227, false);
			KillTimer(m_hWnd, 213);
			speak_button_ont_the_edge_ = true;
			static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("anima_tab")))->SelectItem(2);
		}
	};

	auto timer_214 = [&]() {
		RECT rect;
		GetWindowRect(m_hWnd, &rect);
		int w = GetSystemMetrics(SM_CXSCREEN) - rect.left;
		if (w < 100) {
			MoveWindow(m_hWnd, rect.left - 2, rect.top, w + 2, 227, false);
		}
		else {
			KillTimer(m_hWnd, 214);
		}
	};

	switch (wparam)
	{
	case 211: timer_211(); break;
	case 212: timer_212(); break;
	case 213: timer_213(); break;
	case 214: timer_214(); break;
	default:
		bHandled = false;
		break;
	}

	return LRESULT();
}

LRESULT MainWnd::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	switch (uMsg)
	{
	case WM_MOUSEHOVER:
		if (speak_button_ont_the_edge_) {
			OutputDebugStringA("mouse hover \n");
			SetTimer(m_hWnd, 214, 1, nullptr);
			static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("anima_tab")))->SelectItem(1);
			speak_button_ont_the_edge_ = false;
		}
		break;
	default:
		break;
	}
	return LRESULT();
}

void MainWnd::WebClientInit()
{
	have_server_ip_ = false;

	/* 获取服务器IP，判断是否需要设置IP信息 */
	auto xml_mnge = App::GetInstance()->GetXmlMnge();
	std::string server_ip = CW2A(xml_mnge->GetNodeAttr(_T("ServerIp"), _T("value")).GetData());
	std::string server_port = CW2A(xml_mnge->GetNodeAttr(_T("ServerPort"), _T("value")).GetData());
	local_ip_ = CW2A(xml_mnge->GetNodeAttr(_T("LocalIP"), _T("value")).GetData());

	if (server_ip == "" || server_port == "" || local_ip_ == "") {
		if (MessageBox(m_hWnd, _T("IP配置尚未完善，是否进行设置？"), _T("Message"), MB_YESNO) == IDYES) {
			SetupWnd setup_wnd(m_hWnd);
			setup_wnd.DoModal(login_hwnd_);
		}
	} else {
		have_server_ip_ = true;
		web_client_->Initial(server_ip + ":" + server_port);	// 初始化 web 客户端 
	}
}

bool MainWnd::Login()
{
	/* 如果服务器 IP 未设置好，则重新设置并初始化 Web 客户端 */
	if (!have_server_ip_) {
		WebClientInit();
		if (!have_server_ip_)
			return false;
	}

	/* 判断登录信息是否合理 */
	login_hwnd_ = m_hWnd;
	CDuiString sno = m_pm.FindControl(_T("Sno"))->GetText();
	CDuiString name = m_pm.FindControl(_T("name"))->GetText();
	if (sno == _T("学号") || sno == _T("") || name == _T("姓名") || name == _T("")) {
		MessageBox(m_hWnd, _T("请确认登录信息！"), _T("Message"), MB_OK);
		return false;
	}

	stu_info_.sno_ = CW2A(sno.GetData());
	stu_info_.naem_ = CW2A(name.GetData());

	/* 组装登录信息,登录服务器 */
	stu_info_.operate_type_ = OperateType::LOGON;
	stu_info_.stream_ip_ = local_ip_;
	web_client_->SendWebMessage(json_operate_->AssembleJson(stu_info_));
	//LoginAnimation();
	return true;
}

void MainWnd::LoginAnimation()
{
	m_pm.FindControl(_T("mask_label"))->SetVisible(false);
	static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("anima_tab")))->SelectItem(1);

	int w = 100, h = 227;
	int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
	MoveWindow(m_hWnd, x, y, w, h, true);
	SetTimer(m_hWnd, 212, 5, NULL);
}

void MainWnd::Speak()
{
	// 请求发言；
	stu_info_.handup_ = true;
	stu_info_.operate_type_ = OperateType::SPEAK;
	rpc_client_->HandupOperat(json_operate_->AssembleJson(stu_info_));
}

void MainWnd::StopSpeak()
{
	if (stu_info_.handup_) {
		stu_info_.handup_ = false;
		stu_info_.operate_type_ = OperateType::STOP_SPEAK;
		rpc_client_->HandupOperat(json_operate_->AssembleJson(stu_info_));
	}
}

bool MainWnd::HandUp()
{
	stu_info_.handup_ = true;
	stu_info_.operate_type_ = OperateType::HANDUP;
	rpc_client_->HandupOperat(json_operate_->AssembleJson(stu_info_));
	return false;
}

void MainWnd::AddTray()
{
	// 创建、初始化 托盘菜单窗体
	tray_menu_.reset(new MenuWnd(m_hWnd));
	tray_menu_->CreateWithDefaltStyle();

	tray_data_.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	tray_data_.hWnd = this->m_hWnd;
	tray_data_.uID = 1;
	tray_data_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	tray_data_.uCallbackMessage = kAM_TrayCallbackMsg;
	tray_data_.hIcon = LoadIcon((HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDI_ICON1));

	wcscpy_s(tray_data_.szTip, L"iVGA");
	Shell_NotifyIcon(NIM_ADD, &tray_data_);
}

void MainWnd::OnCloseMsg()
{
	StopSpeak();		// 退出程序前，先结束发言；
	Close();
}
