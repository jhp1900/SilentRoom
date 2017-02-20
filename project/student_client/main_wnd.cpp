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
	AddTray();		// 添加托盘
	GetLocalIP();	// 获取本机ip

	json_operate_.reset(new JsonOperate);
	web_client_.reset(new WebStudentClient);

	/* 鼠标事件初始化 */
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
	/* 判断登录信息是否合理 */
	login_hwnd_ = m_hWnd;
	CDuiString sno = m_pm.FindControl(_T("Sno"))->GetText();
	CDuiString name = m_pm.FindControl(_T("name"))->GetText();
	if (sno == _T("学号") || sno == _T("") || name == _T("姓名") || name == _T("")) {
		MessageBox(m_hWnd, _T("请确认登录信息！"), _T("Message"), MB_OK);
		return false;
	}

	/* 获取服务器IP，判断是否需要设置IP信息 */
	std::string server_ip = CW2A(App::GetInstance()->GetXmlMnge()->GetNodeAttr(_T("ServerIp"), _T("value")).GetData());
	if (server_ip == "") {
		if (MessageBox(m_hWnd, _T("尚未设置服务器IP，是否进行设置？"), _T("Message"), MB_YESNO) == IDYES) {
			SetupWnd setup_wnd(m_hWnd);
			setup_wnd.DoModal(login_hwnd_);
		}
		return false;
	}

	stu_info_.sno_ = CW2A(sno.GetData());
	stu_info_.naem_ = CW2A(name.GetData());

	/* 组装登录信息,登录服务器 */
	web_client_->Initial("http://10.18.3.62:8081");	// 初始化 web 客户端 
	web_client_->SendWebMessage(json_operate_->AssembleJson(stu_info_));
	
	// TODO....
	// 接收登录操作返回值

	if (true) {			// 如果登录成功
		login_hwnd_ = NULL;		// 登录成功后，"设置界面的窗口"不再以本窗口为父窗口！

		// 登录动效
		LoginAnimation();

		// 初始化、启动 ivga
		if (!App::GetInstance()->GetVLCTool()->BeginBroadcast(local_ip_))
			MessageBox(m_hWnd, _T("屏幕推流失败!"), _T("Message"), MB_OK);

		// TODO... 
		// 初始化、启动 rpc client
		rpc_client_.reset(new RpcClient);
		rpc_client_->BindRpcServer("10.18.3.62", "12322");
	} else {
		MessageBox(m_hWnd, _T("登录失败！"), _T("Message"), MB_OK);
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
	// 请求发言；
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

void MainWnd::GetLocalIP()
{
	AutoGetIp();
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
				//MessageBoxA(m_hWnd, oss.str().c_str(), "Message", MB_OK);
				local_ip_ = oss.str();
				iter = iter->Next;
			}
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
		delete pIpAdapterInfo;	// 释放掉
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
		shell_info.lpVerb = _T("runas");      // runas 可以实现以管理员权限运行程序
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
		MessageBox(nullptr, _T("网络检测失败！"), _T("ERROR"), MB_OK);
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
