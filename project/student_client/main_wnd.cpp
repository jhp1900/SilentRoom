#include "main_wnd.h"
#include <math.h>
#include "application.h"
#include "resource.h"
#include "vlc_tool.h"
#include <atlbase.h>
#include "..\utils\setup_wnd.h"
#include "..\utils\xml_manager.h"
#include "..\utils\utils.h"
#include <tchar.h>

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

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
	, wnd_w_(100)
	, wnd_h_(227)
	, hide_w_(6)
	, ep_y_(150)
{
	ep_x_ = GetSystemMetrics(SM_CXSCREEN) - wnd_w_;
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

	if (!InitNativeWifi())
		return;

	json_operate_.reset(new JsonOperate);
	web_client_.reset(new WebStudentClient);

	///* 鼠标事件初始化 */
	track_mouse_event_.cbSize = sizeof(TRACKMOUSEEVENT);
	track_mouse_event_.dwFlags = TME_LEAVE | TME_HOVER;
	track_mouse_event_.hwndTrack = m_hWnd;
	track_mouse_event_.dwHoverTime = 10;

	RegisterHotKey(m_hWnd, 1, MOD_CONTROL, '1');
	RegisterHotKey(m_hWnd, 2, MOD_CONTROL, '2');
	RegisterHotKey(m_hWnd, 3, MOD_CONTROL, '3');
	RegisterHotKey(m_hWnd, 4, MOD_CONTROL, 'X');
	login_hwnd_ = m_hWnd;
	//WebClientInit();
}

void MainWnd::OnClickBtn(TNotifyUI & msg, bool & handled)
{
	CDuiString name = msg.pSender->GetName();
	if (name == _T("close_btn") || name == _T("closebtn"))
		OnCloseMsg();
	else if (name == _T("login_btn"))
		Login();
	else if (name == _T("speak_item"))
		Speak();
	else if (name == _T("handup_item"))
		HandUp();
	else if (name == _T("stop_item"))
		StopSpeak();
	else if (name == _T("setup_item")) {
		SetupWnd setup_wnd(m_hWnd);
		setup_wnd.DoModal(login_hwnd_);
	}
	else if (name == L"conn_btn") {
		OnConnBtn();
	}
	else if (name == L"chioce_ip_btn") {
		auto ip_list = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("ip_list")));
		if (ip_list)
			OnChioceIp(ip_list->GetText());
	}
	else if (name == L"overlap_btn") {
		PostMessage(kAM_ChoiceNICMsg, 0, 0);
	}
	else if (name == L"wifi_set_btn") {
		
	}
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
	DWORD dwexstyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	dwexstyle ^= WS_EX_TOOLWINDOW;
	SetWindowLong(m_hWnd, GWL_EXSTYLE, dwexstyle);
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

LRESULT MainWnd::OnChoiceNICMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("Log_tab")))->SelectItem(1);
	std::vector<std::wstring> ips;
	GetLocalIP(ips);
	
	if (ips.size() > 1) {
		CComboBoxUI *ip_list = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("ip_list")));
		for (auto ip : ips) {
			auto elemen = new CListLabelElementUI;
			elemen->SetText(ip.c_str());
			ip_list->Add(elemen);
		}
	}
	else if(ips.size() == 1){
		if (!OnChioceIp(ips.at(0).c_str())) {
			CComboBoxUI *ip_list = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("ip_list")));
			auto elemen = new CListLabelElementUI;
			elemen->SetText(ips.at(0).c_str());
			ip_list->Add(elemen);
		}
	}
	else {
		m_pm.FindControl(L"ip_prompt_msg")->SetVisible(true);
	}

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
	auto timer_211 = [&]() {
		OutputDebugStringA("time up\n");
		SetTimer(m_hWnd, 213, 1, nullptr);
		KillTimer(this->m_hWnd, 211);
	};

	auto timer_212 = [&]() {
		RECT rect;
		GetWindowRect(m_hWnd, &rect);
		int add_x = (ep_x_ - rect.left) / 10;
		int add_y = (rect.top - ep_y_) / 10;
		if (add_x > 1 || add_x < -1 || add_y > 1 || add_y < -1) {
			int x = rect.left + add_x;
			int y = rect.top - add_y;
			MoveWindow(m_hWnd, x, y, wnd_w_, wnd_h_, true);
		}
		else {
			KillTimer(this->m_hWnd, 212);
			MoveWindow(m_hWnd, ep_x_, ep_y_, wnd_w_, wnd_h_, true);
		}
	};

	auto timer_213 = [&]() {
		RECT rect;
		GetWindowRect(m_hWnd, &rect);
		int w = GetSystemMetrics(SM_CXSCREEN) - rect.left;
		if (w > hide_w_) {
			MoveWindow(m_hWnd, rect.left + 2, ep_y_, w - 2, wnd_h_, false);
		}
		else{
			MoveWindow(m_hWnd, GetSystemMetrics(SM_CXSCREEN) - hide_w_, ep_y_, hide_w_, wnd_h_, false);
			KillTimer(m_hWnd, 213);
			speak_button_ont_the_edge_ = true;
			static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("anima_tab")))->SelectItem(2);
		}
	};

	auto timer_214 = [&]() {
		RECT rect;
		GetWindowRect(m_hWnd, &rect);
		int w = GetSystemMetrics(SM_CXSCREEN) - rect.left;
		if (w < wnd_w_) {
			MoveWindow(m_hWnd, rect.left - 2, ep_y_, w + 2, wnd_h_, false);
		}
		else {
			KillTimer(m_hWnd, 214);
			MoveWindow(m_hWnd, GetSystemMetrics(SM_CXSCREEN) - wnd_w_, ep_y_, wnd_w_, wnd_h_, false);
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

bool MainWnd::WebClientInit()
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
		return web_client_->Initial(server_ip + ":" + server_port);	// 初始化 web 客户端 
	}
	return false;
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

void MainWnd::Logout()
{
	stu_info_.operate_type_ = OperateType::QUIT;
	web_client_->SendWebMessage(json_operate_->AssembleJson(stu_info_));
}

void MainWnd::LoginAnimation()
{
	m_pm.FindControl(_T("mask_label"))->SetVisible(false);
	static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("anima_tab")))->SelectItem(1);

	int x = (GetSystemMetrics(SM_CXSCREEN) - wnd_w_) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - wnd_h_) / 2;
	MoveWindow(m_hWnd, x, y, wnd_w_, wnd_h_, true);
	SetTimer(m_hWnd, 212, 5, NULL);
}

void MainWnd::Speak()
{
	// 请求发言；
	stu_info_.handup_ = true;
	stu_info_.operate_type_ = OperateType::SPEAK;
	if(!rpc_client_->HandupOperat(json_operate_->AssembleJson(stu_info_)))
		stu_info_.handup_ = false;
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
	if(!rpc_client_->HandupOperat(json_operate_->AssembleJson(stu_info_)))
		stu_info_.handup_ = false;
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
	Logout();			// 向服务器发出登出信息；
	Close();
}

bool MainWnd::InitNativeWifi()
{
	DWORD dwResult = 0;
	DWORD dwMaxClien = 2;
	DWORD dwCurVersion = 0;

	dwResult = WlanOpenHandle(dwMaxClien, NULL, &dwCurVersion, &hClient_);
	if (dwResult != ERROR_SUCCESS) {
		WCHAR out[MAX_PATH];
		_stprintf_s(out, L"WlanOpenHandle failed with error: %u\n", dwResult);
		OutputDebugString(out);
		return false;
	}

	PWLAN_INTERFACE_INFO_LIST pIfList = nullptr;
	PWLAN_INTERFACE_INFO pIfInfo = nullptr;

	dwResult = WlanEnumInterfaces(hClient_, nullptr, &pIfList);
	if (dwResult != ERROR_SUCCESS) {
		WCHAR out[MAX_PATH];
		_stprintf_s(out, L"WlanEnumInterfaces failed with error: %u\n", dwResult);
		OutputDebugString(out);
		return false;
	}

	if (pIfList->dwNumberOfItems == 1) {		// 如果只有一张无线网卡，且连接成功，则直接跳转到IP选择界面
		if (ConnectWifi(pIfList->InterfaceInfo[0].InterfaceGuid)) {
			PostMessage(kAM_ChoiceNICMsg, 0, 0);
			return true;
		}
	}
	else if (pIfList->dwNumberOfItems == 0) {	// 如果没有无线网卡，则给个提示
		m_pm.FindControl(L"wlan_prompt_msg")->SetVisible(true);
	}

	std::vector<std::pair<GUID, std::wstring>> all_nic;
	if (!GetNICInfo(all_nic))
		return false;

	for (int index_card = 0; index_card < pIfList->dwNumberOfItems; ++index_card) {
		pIfInfo = &pIfList->InterfaceInfo[index_card];
		for (auto item : all_nic) {
			if (item.first == pIfInfo->InterfaceGuid) {
				wlan_nic_.push_back(item);
				break;
			}
		}

		//PWLAN_AVAILABLE_NETWORK_LIST available_list = nullptr;
		//dwResult = WlanGetAvailableNetworkList(hClient, &pIfInfo->InterfaceGuid, WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_MANUAL_HIDDEN_PROFILES, nullptr, &available_list);
		//if (available_list != nullptr) {
		//	for (int index_network = 0; index_network < available_list->dwNumberOfItems; ++index_network) {
		//		WLAN_AVAILABLE_NETWORK network = available_list[index_network].Network[0];
		//		int a = 0;
		//	}
		//}
	}

	CComboBoxUI *wlan_list = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("wlan_list")));
	if (!wlan_list)
		return false;
	for (auto item : wlan_nic_) {
		auto elemen = new CListLabelElementUI;
		elemen->SetText(item.second.c_str());
		wlan_list->Add(elemen);
	}

	return true;
}

bool MainWnd::ConnectWifi(GUID guid)
{
	DWORD dwResult = 0;
	DWORD dwFlags;
	DWORD pdwGrantedAccess;
	LPWSTR xmlfile;
	dwResult = WlanGetProfile(hClient_, &guid, L"TingFox", nullptr, &xmlfile, &dwFlags, &pdwGrantedAccess);
	if (dwResult != ERROR_SUCCESS) {
		LPCWSTR profileXml;
		std::wstring strHead = L"<?xml version=\"1.0\"?>\
				<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">\
				<name>TingFox</name>\
				<SSIDConfig>\
				<SSID>\
				<name>TingFox</name>\
				</SSID>\
				</SSIDConfig>\
				<connectionType>ESS</connectionType>\
				<connectionMode>auto</connectionMode>\
				<MSM>\
				<security>\
				<authEncryption>\
				<authentication>WPA2PSK</authentication>\
				<encryption>AES</encryption>\
				<useOneX>false</useOneX>\
				</authEncryption>\
				<sharedKey>\
				<keyType>passPhrase</keyType>\
				<protected>false</protected>\
				<keyMaterial>tingfox876718</keyMaterial>\
				</sharedKey>\
				</security>\
				</MSM>\
				</WLANProfile>";

		profileXml = strHead.c_str();
		dwFlags = 0;
		WLAN_REASON_CODE dwReasonCode;
		dwResult = WlanSetProfile(hClient_, &guid, dwFlags, profileXml, nullptr, true, nullptr, &dwReasonCode);
		if (dwResult != ERROR_SUCCESS)
			OutputDebugString(L"\n- - - - - 设置配置文件失败！ - - - - -\n");
	}

	// 通过以保存的配置文件连接
	if (dwResult == ERROR_SUCCESS) {
		WLAN_CONNECTION_PARAMETERS connect_param;
		connect_param.wlanConnectionMode = wlan_connection_mode_profile;
		connect_param.strProfile = L"TingFox";
		connect_param.pDot11Ssid = nullptr;
		connect_param.dot11BssType = dot11_BSS_type_infrastructure;
		connect_param.pDesiredBssidList = nullptr;
		connect_param.dwFlags = WLAN_CONNECTION_HIDDEN_NETWORK;

		dwResult = WlanConnect(hClient_, &guid, &connect_param, nullptr);
		WCHAR out[MAX_PATH];
		if (dwResult == ERROR_SUCCESS) {
			_stprintf_s(out, L"\n- - - - - 连接成功！ - - - - -\n");
			OutputDebugString(out);
			return true;
		}
		else {
			_stprintf_s(out, L"\n- - - - - 连接失败！ - - - - -\n");
			OutputDebugString(out);
			return false;
		}
	}

	return false;
}

bool MainWnd::OnConnBtn()
{
	CComboBoxUI *wlan_list = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("wlan_list")));
	if (!wlan_list)
		return false;

	CDuiString nic_name = wlan_list->GetText();
	if (wlan_list->GetText() == L"")
		return false;

	for (auto item : wlan_nic_) {
		if (nic_name == item.second.c_str())
			if (ConnectWifi(item.first)) {
				PostMessage(kAM_ChoiceNICMsg, 0, 0);
				return true;
			}
			else {
				return false;
			}
	}

	return false;
}

bool MainWnd::OnChioceIp(LPCTSTR ip)
{
	auto xml_mnge = App::GetInstance()->GetXmlMnge();
	if (xml_mnge->SetNodeAttr(_T("LocalIP"), _T("value"), ip)) {
		
		if (WebClientInit()) {
			static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("Log_tab")))->SelectItem(0);
			return true;
		}
	}
	return false;
}
