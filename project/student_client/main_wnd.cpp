#include "main_wnd.h"
#include <math.h>
#include "application.h"
#include "resource.h"
#include "vlc_tool.h"
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <sstream>
#include <NetCon.h>
#include <atlbase.h>
#include "setup_wnd.h"
#include "xml_manager.h"

#pragma comment(lib,"Iphlpapi.lib")

MainWnd::MainWnd()
	: tray_data_({0})
	, local_ip_("")
	, stu_info_()
	, rpc_client_(nullptr)
	, old_point_({0})
	, cursor_point_({0})
	, move_now_(false)
	, login_hwnd_(nullptr)
{
	stu_info_.appid_ = appid_str;
}

MainWnd::~MainWnd()
{
	Shell_NotifyIcon(NIM_DELETE, &tray_data_);
}

void MainWnd::InitWindow()
{
	AddTray();		// �������
	GetLocalIP();	// ��ȡ����ip

	json_operate_.reset(new JsonOperate);
	web_client_.reset(new WebStudentClient);

	/* ����¼���ʼ�� */
	track_mouse_event_.cbSize = sizeof(TRACKMOUSEEVENT);
	track_mouse_event_.dwFlags = TME_LEAVE;
	track_mouse_event_.hwndTrack = m_hWnd;

	login_hwnd_ = m_hWnd;
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
	if (lparam == WM_RBUTTONUP)
	{
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

bool MainWnd::Login()
{
	/* �жϵ�¼��Ϣ�Ƿ���� */
	login_hwnd_ = m_hWnd;
	CDuiString sno = m_pm.FindControl(_T("Sno"))->GetText();
	CDuiString name = m_pm.FindControl(_T("name"))->GetText();
	if (sno == _T("ѧ��") || sno == _T("") || name == _T("����") || name == _T("")) {
		MessageBox(m_hWnd, _T("��ȷ�ϵ�¼��Ϣ��"), _T("Message"), MB_OK);
		return false;
	}

	/* ��ȡ������IP���ж��Ƿ���Ҫ����IP��Ϣ */
	std::string server_ip = CW2A(App::GetInstance()->GetXmlMnge()->GetNodeAttr(_T("ServerIp"), _T("value")).GetData());
	if (server_ip == "") {
		if (MessageBox(m_hWnd, _T("��δ���÷�����IP���Ƿ�������ã�"), _T("Message"), MB_YESNO) == IDYES) {
			SetupWnd setup_wnd(m_hWnd);
			setup_wnd.DoModal(login_hwnd_);
		}
		return false;
	}

	stu_info_.sno_ = CW2A(sno.GetData());
	stu_info_.naem_ = CW2A(name.GetData());

	/* ��װ��¼��Ϣ,��¼������ */
	web_client_->Initial("http://10.18.3.62:8081");	// ��ʼ�� web �ͻ��� 
	web_client_->SendWebMessage(json_operate_->AssembleJson(stu_info_));
	
	// TODO....
	// ���յ�¼��������ֵ

	if (true) {			// �����¼�ɹ�
		login_hwnd_ = NULL;		// ��¼�ɹ���"���ý���Ĵ���"�����Ա�����Ϊ�����ڣ�

		// ��¼��Ч
		LoginAnimation();

		// ��ʼ�������� ivga
		if (!App::GetInstance()->GetVLCTool()->BeginBroadcast(local_ip_))
			MessageBox(m_hWnd, _T("��Ļ����ʧ��!"), _T("Message"), MB_OK);

		// TODO... 
		// ��ʼ�������� rpc client
		rpc_client_.reset(new RpcClient);
		rpc_client_->BindRpcServer("10.18.3.62", "12322");
	} else {
		MessageBox(m_hWnd, _T("��¼ʧ�ܣ�"), _T("Message"), MB_OK);
	}
	
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
	// �����ԣ�
	stu_info_.handup_ = true;
	rpc_client_->HandupOperat(json_operate_->AssembleJson(stu_info_));
}

void MainWnd::StopSpeak()
{
	stu_info_.handup_ = false;
}

bool MainWnd::HandUp()
{
	return false;
}

void MainWnd::AddTray()
{
	// ��������ʼ�� ���̲˵�����
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

void MainWnd::GetLocalIP()
{
	AutoGetIp();
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
				//MessageBoxA(m_hWnd, oss.str().c_str(), "Message", MB_OK);
				local_ip_ = oss.str();
				iter = iter->Next;
			}
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
		delete pIpAdapterInfo;	// �ͷŵ�
	}
}

void MainWnd::AutoGetIp()
{
	auto set_ip = [](LPCTSTR type, LPCTSTR net_name) {
		CDuiString comline = _T("/c netsh interface ip set name=\"");
		comline += type;
		comline += _T("\" ");
		comline += net_name;
		comline += _T(" dhcp");

		SHELLEXECUTEINFO shell_info = { 0 };
		shell_info.cbSize = sizeof(SHELLEXECUTEINFO);
		shell_info.fMask = SEE_MASK_NOCLOSEPROCESS;
		shell_info.lpVerb = _T("runas");      // runas ����ʵ���Թ���ԱȨ�����г���
		shell_info.hwnd = nullptr;
		shell_info.lpFile = _T("cmd.exe");
		shell_info.lpDirectory = nullptr;
		shell_info.nShow = SW_HIDE;
		shell_info.lpParameters = comline;
		shell_info.hInstApp = nullptr;

		//DWORD exitcode;
		ShellExecuteEx(&shell_info);
	};

	INetConnectionManager *net_manager = nullptr;
	IEnumNetConnection *net_enum = nullptr;
	INetConnection *net_conn = nullptr;
	ULONG celt_fetched;

	setlocale(LC_CTYPE, "");
	CoInitialize(NULL);
	CoCreateInstance(CLSID_ConnectionManager, NULL, CLSCTX_SERVER, IID_INetConnectionManager, (void**)&net_manager);
	if (net_manager == nullptr) {
		MessageBox(nullptr, _T("������ʧ�ܣ�"), _T("ERROR"), MB_OK);
		return;
	}

	net_manager->EnumConnections(NCME_DEFAULT, &net_enum);
	NETCON_PROPERTIES *net_proper;
	while (net_enum->Next(1, &net_conn, &celt_fetched) == S_OK)
	{
		net_conn->GetProperties(&net_proper);
		set_ip(_T("address"), net_proper->pszwName);
		set_ip(_T("dnsservers"), net_proper->pszwName);
	}
}
