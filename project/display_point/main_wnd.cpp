#include "main_wnd.h"
#include "resource.h"
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <sstream>
#include <NetCon.h>
#include "application.h"
#include "vlc_tool.h"

#pragma comment(lib,"Iphlpapi.lib")

MainWnd::MainWnd()
	: tray_data_({0})
	, ip_info_("")
	, show_wnd_(false)
	, tray_menu_(nullptr)
{
}

MainWnd::~MainWnd()
{
	Shell_NotifyIcon(NIM_DELETE, &tray_data_);
}

void MainWnd::InitWindow()
{
	AddTray();		// �������
	GetLocalIP();	// ��ȡ����IP

	// ��������ʼ�� ���̲˵�
	tray_menu_.reset(new MenuWnd(m_hWnd));
	tray_menu_->CreateWithDefaltStyle();
	
	// TODO ...
	// ���� RPC ������

	// ��ʼ�������� ivga �㲥
	if (!App::GetInstance()->GetVLCTool()->BeginBroadcast(ip_info_))
		MessageBox(m_hWnd, _T("��Ļ����ʧ��!"), _T("Message"), MB_OK);
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
		case WM_LBUTTONUP:
			if (show_wnd_)
				ShowWindow(SW_HIDE);
			else
				ShowWindow(SW_SHOW);
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
			ShowWindow(SW_SHOW);
			show_wnd_ = true;
			App::GetInstance()->GetVLCTool()->PlayStream(m_hWnd, "rtsp://10.18.3.62:554/live");
			break;
		case MenuMsgTemp2:
			App::GetInstance()->GetVLCTool()->DestoryPlay();
			ShowWindow(SW_HIDE);
			show_wnd_ = false;
			break;
		default:
			break;
	}
	return LRESULT();
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

void MainWnd::GetLocalIP()
{
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	// ���stSize ����������Ҳ���������
	if (ERROR_BUFFER_OVERFLOW == nRel) {
		// ERROR_BUFFER_OVERFLOW ��ʾ ���ݸ� GetAdaptersInfo ���ڴ�ռ䲻����ͬʱ������ stSize ��ʾ����Ŀռ��С
		delete pIpAdapterInfo;	// �ͷŵ������·��䣡
		pIpAdapterInfo = (PIP_ADAPTER_INFO) new BYTE[stSize];
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	// ���ô������¿ռ��Сֵ��������� pIpAdapterInfo
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
		delete pIpAdapterInfo;	// �ͷŵ�
	}
}