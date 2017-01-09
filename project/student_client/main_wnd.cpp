#include "main_wnd.h"
#include <math.h>
#include "application.h"
#include "resource.h"
#include "vlc_tool.h"
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <sstream>
#include <NetCon.h>

#pragma comment(lib,"Iphlpapi.lib")

MainWnd::MainWnd()
	: tray_data_({0})
	, ip_info_("")
{
}

MainWnd::~MainWnd()
{
	Shell_NotifyIcon(NIM_DELETE, &tray_data_);
}

void MainWnd::InitWindow()
{
	// �������
	AddTray();

	// ��ȡ����ip
	GetLocalIP();
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
	// ����һ����¼��Ϣ�������շ���ֵ

	if (true) {			// �����¼�ɹ�
		// ��¼��Ч
		LoginAnimation();

		// ��ʼ�������� ivga
		if (!App::GetInstance()->GetVLCTool()->BeginBroadcast(ip_info_))
			MessageBox(m_hWnd, _T("��Ļ����ʧ��!"), _T("Message"), MB_OK);

		// TODO... 
		// ��ʼ�������� rpc client
	} else {
		MessageBox(m_hWnd, _T("��¼ʧ�ܣ�"), _T("Message"), MB_OK);
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
		Sleep(3);
	}
}

void MainWnd::Speak()
{
	// TODO... 
	// �����ԣ�
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
				MessageBoxA(m_hWnd, oss.str().c_str(), "Message", MB_OK);
				ip_info_ = oss.str();
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
		CDuiString comline = _T("/c netsh interface ip set ");
		comline += type;
		comline += _T(" ");
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

		DWORD exitcode;
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
		MessageBox(m_hWnd, net_proper->pszwName, _T("Message"), MB_OK);
	}
}
