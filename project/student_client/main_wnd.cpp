#include "main_wnd.h"
#include <math.h>
#include "application.h"
#include "resource.h"
#include "vlc_tool.h"

MainWnd::MainWnd()
	: tray_data_({0})
{
}

MainWnd::~MainWnd()
{
	Shell_NotifyIcon(NIM_DELETE, &tray_data_);
}

void MainWnd::InitWindow()
{
	// 添加托盘
	AddTray();

	// TODO...
	// 获取本机ip
}

void MainWnd::OnClickBtn(TNotifyUI & msg, bool & handled)
{
	if (msg.pSender->GetName() == _T("close_btn"))
		::PostQuitMessage(0);
	else if (msg.pSender->GetName() == _T("login_btn"))
		Login();
	else if (msg.pSender->GetName() == _T("speak_btn"))
		Speak();
}

LRESULT MainWnd::OnTray(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	return LRESULT();
}

bool MainWnd::Login()
{
	// TODO....
	// 发送一个登录消息，并接收返回值

	if (true) {			// 如果登录成功
		LoginAnimation();

		// TODO... 
		// 初始化、启动 rpc client

		// TODO... 
		// 初始化、启动 ivga
		VLCTool vlc_tool;
		if (!vlc_tool.BeginBroadcast("10.18.3.62"))
			MessageBox(m_hWnd, _T("屏幕推流失败!"), _T("Message"), MB_OK);
	} else {
		MessageBox(m_hWnd, _T("登录失败！"), _T("Message"), MB_OK);
	}
	
	return true;
}

void MainWnd::LoginAnimation()
{
	static_cast<CAnimationTabLayoutUI*>(m_pm.FindControl(_T("anima_tab")))->SelectItem(1);

	RECT rect = { 0 };
	GetWindowRect(m_hWnd, &rect);
	POINT centre = { 0 }, top = { 0 };
	centre.x = (rect.right + rect.left) / 2;
	centre.y = (rect.bottom + rect.top) / 2;
	for (int i = (rect.right - rect.left) / 2; i >= 30; --i) {
		MoveWindow(m_hWnd, centre.x - i, centre.y - i, i * 2, i * 2, true);
		Sleep(1);
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
		Sleep(3);
	}
}

void MainWnd::Speak()
{
	// TODO... 
	// 请求发言；
}

void MainWnd::AddTray()
{
	tray_data_.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	tray_data_.hWnd = this->m_hWnd;
	tray_data_.uID = 1;
	tray_data_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	tray_data_.uCallbackMessage = kAM_TrayCallbackMsg;
	tray_data_.hIcon = LoadIcon((HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDI_ICON1));

	wcscpy_s(tray_data_.szTip, L"iVGA");
	Shell_NotifyIcon(NIM_ADD, &tray_data_);
}
