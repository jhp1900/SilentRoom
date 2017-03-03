#include "main_wnd.h"
#include <math.h>
#include "application.h"
#include "resource.h"
#include "vlc_tool.h"
#include <atlbase.h>
#include "setup_wnd.h"
#include "xml_manager.h"
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
{
	stu_info_.appid_ = appid_str;
}

MainWnd::~MainWnd()
{
	Shell_NotifyIcon(NIM_DELETE, &tray_data_);
}

void MainWnd::InitWindow()
{
	AddTray();				// 添加托盘
	SetAutoGetIP();			// 设置电脑为自动获取IP
	GetLocalIP(local_ip_);	// 获取本机ip

	json_operate_.reset(new JsonOperate);
	web_client_.reset(new WebStudentClient);

	/* 鼠标事件初始化 */
	track_mouse_event_.cbSize = sizeof(TRACKMOUSEEVENT);
	track_mouse_event_.dwFlags = TME_LEAVE;
	track_mouse_event_.hwndTrack = m_hWnd;

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
			Close();
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
	std::string ret_data = *(std::string*)(wparam);
	LogonInfo logon_info;
	json_operate_->JsonAnalysis(ret_data.c_str(), logon_info);

	login_hwnd_ = NULL;		// 登录成功后，"设置界面的窗口"不再以本窗口为父窗口！
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

LRESULT MainWnd::OnMouseMoveWnd(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	if (uMsg == WM_RBUTTONDOWN) {
		::GetCursorPos(&old_point_);
		RECT rect = m_pm.FindControl(_T("speak_btn"))->GetPos();

		if (rect.right > 0 && rect.bottom > 0) {
			move_now_ = true;
			TrackMouseEvent(&track_mouse_event_);
		}
	} else if (uMsg == WM_RBUTTONUP || uMsg == WM_MOUSELEAVE) {
		move_now_ = false;
	} else if (uMsg == WM_MOUSEMOVE) {
		if (move_now_) {
			::GetCursorPos(&cursor_point_);
			RECT rect = { 0 };
			GetWindowRect(m_hWnd, &rect);
			int w = rect.right - rect.left;
			int h = rect.bottom - rect.top;
			rect.left += (cursor_point_.x - old_point_.x);
			rect.top += (cursor_point_.y - old_point_.y);
			old_point_.x = cursor_point_.x;
			old_point_.y = cursor_point_.y;

			MoveWindow(m_hWnd, rect.left, rect.top, w, h, false);
		}
	}
	return LRESULT();
}

LRESULT MainWnd::OnNcLButDbClk(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	return LRESULT();
}

void MainWnd::WebClientInit()
{
	/* 获取服务器IP，判断是否需要设置IP信息 */
	std::string server_ip = CW2A(App::GetInstance()->GetXmlMnge()->GetNodeAttr(_T("ServerIp"), _T("value")).GetData());
	if (server_ip == "") {
		if (MessageBox(m_hWnd, _T("尚未设置服务器IP，是否进行设置？"), _T("Message"), MB_YESNO) == IDYES) {
			SetupWnd setup_wnd(m_hWnd);
			setup_wnd.DoModal(login_hwnd_);
		}
		have_server_ip_ = false;
	} else {
		have_server_ip_ = true;
		web_client_->Initial(server_ip + ":8082");	// 初始化 web 客户端 
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
	stu_info_.operate_type_ = LOGON;
	web_client_->SendWebMessage(json_operate_->AssembleJson(stu_info_));

	return true;
}

void MainWnd::LoginAnimation()
{
	m_pm.FindControl(_T("mask_label"))->SetVisible(false);
	static_cast<CAnimationTabLayoutUI*>(m_pm.FindControl(_T("anima_tab")))->SelectItem(1);

	RECT rect = { 0 };
	GetWindowRect(m_hWnd, &rect);
	POINT centre = { 0 }, top = { 0 };
	centre.x = (rect.right + rect.left) / 2;
	centre.y = (rect.bottom + rect.top) / 2;
	for (int i = (rect.right - rect.left) / 2; i >= 30; --i) {
		MoveWindow(m_hWnd, centre.x - i, centre.y - i, i * 2, i * 2, true);
		//Sleep(1);
	}

	int x = GetSystemMetrics(SM_CXSCREEN) - 110;
	int temp = 0;
	bool is_break = false;
	while (true) {
		GetWindowRect(m_hWnd, &rect);
		top.x = rect.left;
		top.y = rect.top;
		temp = abs(x - top.x);
		if (temp) {
			temp /= 10;
			if (temp)
				top.x += temp;
			else
				top.x += 1;
		} else {
			is_break = true;
		}

		temp = abs(top.y - 50);
		if (temp) {
			temp /= 10;
			if (temp)
				top.y -= temp;
			else
				top.y -= 1;
		} else if(is_break) {
			break;
		}
		
		MoveWindow(m_hWnd, top.x, top.y, 60, 60, true);
		Sleep(2);
	}
	::SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void MainWnd::Speak()
{
	// 请求发言；
	stu_info_.handup_ = false;
	stu_info_.operate_type_ = OperateType::SPEAK;
	rpc_client_->HandupOperat(json_operate_->AssembleJson(stu_info_));
}

void MainWnd::StopSpeak()
{
	stu_info_.handup_ = false;
	stu_info_.operate_type_ = OperateType::STOP_SPEAK;
	rpc_client_->HandupOperat(json_operate_->AssembleJson(stu_info_));
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
