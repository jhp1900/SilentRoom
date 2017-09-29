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
#include "net_tool.h"

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

MainWnd::MainWnd(bool is_admin)
	: is_admin_(is_admin)
	, tray_data_({0})
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
	, wnd_h_(187)
	, hide_w_(6)
	, ep_y_(150)
{
	ep_x_ = GetSystemMetrics(SM_CXSCREEN) - wnd_w_;
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
	if (is_admin_) {
		static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("anima_tab")))->SelectItem(3);
		AdminSetwndInit();
	}

	net_tool_ = make_shared<NetworkTool>();
	AddTray();				// 添加托盘
	InitNic();				// 网络初始化
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
	else if (name == L"overlap_btn") {
		PostMessage(kAM_IPChoiceMsg, 0, 0);
	}
	else if (name == L"chioce_ip_btn") {
		OnChioceIp();
	}
	else if (name == L"wifi_set_btn") {
		OnSetWLANInfo();
	}
	else if(name==L"reload_btn")
		PostMessage(kAM_IPChoiceMsg, 0, 0);
}

void MainWnd::OnIpCheckChanged(TNotifyUI & msg, bool & handled)
{
	auto select = static_cast<CCheckBoxUI*>(msg.pSender)->IsSelected();
	static_cast<IpControlUI*>(m_pm.FindControl(L"ip_addr"))->SetEditState(!select);
	static_cast<IpControlUI*>(m_pm.FindControl(L"gateway_addr"))->SetEditState(!select);
	static_cast<IpControlUI*>(m_pm.FindControl(L"dns_addr"))->SetEditState(!select);
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

LRESULT MainWnd::OnIPChoiceMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("Log_tab")))->SelectItem(1);
	CComboBoxUI *ip_list = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("ip_list")));
	if (!ip_list)
		return 0;
	ip_list->RemoveAll();

	std::vector<std::wstring> ips;
	net_tool_->GetAllLocalIP(ips);

	if (ips.size() == 0) {
		m_pm.FindControl(L"ip_prompt_msg")->SetVisible(true);
		return false;
	}
	else {
		m_pm.FindControl(L"ip_prompt_msg")->SetVisible(false);
	}

	for (auto ip : ips) {
		auto elemen = new CListLabelElementUI;
		elemen->SetText(ip.c_str());
		ip_list->Add(elemen);
	}
	ip_list->SelectItem(0);

	if (ips.size() == 1) {
		OnChioceIp();
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

	auto timer_221 = [&]() {
		KillTimer(m_hWnd, 221);
		PostMessage(kAM_IPChoiceMsg, 0, 0);
	};

	switch (wparam)
	{
	case 211: timer_211(); break;
	case 212: timer_212(); break;
	case 213: timer_213(); break;
	case 214: timer_214(); break;
	case 221: timer_221(); break;
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

bool MainWnd::InitLocalClient(std::string loca_ip)
{
	local_ip_ = loca_ip;
	std::string out_msg = "\n - - - LoaclIP : " + local_ip_ + " - - - \n";
	OutputDebugStringA(out_msg.c_str());
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
	WebClientInit();
	return true;
}

bool MainWnd::WebClientInit()
{
	have_server_ip_ = false;

	/* 获取服务器IP，判断是否需要设置IP信息 */
	auto xml_mnge = App::GetInstance()->GetXmlMnge();
	std::string server_ip = CW2A(xml_mnge->GetNodeAttr(_T("ServerIp"), _T("value")).GetData());
	std::string server_port = CW2A(xml_mnge->GetNodeAttr(_T("ServerPort"), _T("value")).GetData());

	if (server_ip == "" || server_port == "") {
		MessageBox(m_hWnd, L"IP配置尚未完善,请联系管理员", L"Message", MB_OK);
		//if (MessageBox(m_hWnd, _T("IP配置尚未完善，是否进行设置？"), _T("Message"), MB_YESNO) == IDYES) {
		//	SetupWnd setup_wnd(m_hWnd);
		//	setup_wnd.DoModal(login_hwnd_);
		//}
	} else {
		have_server_ip_ = true;
		return web_client_->Initial(server_ip + ":" + server_port);	// 初始化 web 客户端 
	}
	return false;
}

bool MainWnd::AdminSetwndInit()
{
	auto xml_mnge = App::GetInstance()->GetXmlMnge();
	m_pm.FindControl(L"app_id")->SetText(xml_mnge->GetNodeAttr(L"AppID", L"value"));
	m_pm.FindControl(L"wifi_name")->SetText(xml_mnge->GetNodeAttr(L"WLAN", L"name"));
	m_pm.FindControl(L"wifi_pwd")->SetText(xml_mnge->GetNodeAttr(L"WLAN", L"pwd"));

	auto auto_ip_attr = xml_mnge->GetNodeAttrToBool(L"LocalIP", L"auto_ip");
	auto ip_attr = xml_mnge->GetNodeAttr(L"LocalIP", L"ip");
	auto gateway_attr = xml_mnge->GetNodeAttr(L"LocalIP", L"gateway");
	auto dns_attr = xml_mnge->GetNodeAttr(L"LocalIP", L"dns");

	auto auto_ip = static_cast<CCheckBoxUI*>(m_pm.FindControl(L"auto_ip_check"));
	auto ip_addr = static_cast<IpControlUI*>(m_pm.FindControl(L"ip_addr"));
	auto gateway_addr = static_cast<IpControlUI*>(m_pm.FindControl(L"gateway_addr"));
	auto dns_addr = static_cast<IpControlUI*>(m_pm.FindControl(L"dns_addr"));

	auto_ip->Selected(auto_ip_attr);
	ip_addr->SetText(ip_attr);
	gateway_addr->SetText(gateway_attr);
	dns_addr->SetText(dns_attr);

	auto svip_attr = xml_mnge->GetNodeAttr(L"ServerIp", L"value");
	static_cast<IpControlUI*>(m_pm.FindControl(L"svip_addr"))->SetText(svip_attr);
	m_pm.FindControl(L"port")->SetText(xml_mnge->GetNodeAttr(L"ServerPort", L"value"));

	return true;
}

bool MainWnd::Login()
{
	auto xml_mnge = App::GetInstance()->GetXmlMnge();
	auto appid = xml_mnge->GetNodeAttr(L"AppID", L"value");
	if (appid == L"") {
		MessageBox(m_hWnd, L"学生端ID配置有误，请联系管理员", L"Warning", MB_OK);
		return false;
	}
	stu_info_.appid_ = CW2A(appid);

	/* 如果服务器 IP 未设置好，则重新设置并初始化 Web 客户端 */
	if (!have_server_ip_) {
		WebClientInit();
		if (!have_server_ip_)
			return false;
	}

	/* 判断登录信息是否合理 */
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
	stu_info_.operate_type_ = OperateType::LOGOUT;
	web_client_->SendWebMessage(json_operate_->AssembleJson(stu_info_));
}

void MainWnd::LoginAnimation()
{
	//m_pm.FindControl(_T("mask_label"))->SetVisible(false);
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
	if (local_ip_ != "") {	// 若此条件不成立，则证明为初始化本地客户端，则不需执行以下两条语句
		StopSpeak();		// 退出程序前，先结束发言；
		Logout();			// 向服务器发出登出信息；
	}
	Close();
}

bool MainWnd::InitNic()
{
	std::vector<std::pair<GUID, std::wstring>> nics;
	if (!net_tool_->GetWlanNICVector(nics))
		return false;

	if (nics.size() == 0) {
		m_pm.FindControl(L"wlan_prompt_msg")->SetVisible(true);
		return false;
	}

	CComboBoxUI *wlan_list = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("wlan_list")));
	if (!wlan_list)
		return false;

	for (auto item : nics) {
		auto elemen = new CListLabelElementUI;
		elemen->SetText(item.second.c_str());
		wlan_list->Add(elemen);
	}
	wlan_list->SelectItem(0);

	if (nics.size() == 1) {
		OnConnBtn();
	}

	return true;
}

bool MainWnd::ConneWifiForNic(LPCTSTR nic_name)
{
	std::vector<std::pair<GUID, std::wstring>> nics;
	if (!net_tool_->GetWlanNICVector(nics))
		return false;

	auto xml_mnge = App::GetInstance()->GetXmlMnge();
	auto wifi_name = xml_mnge->GetNodeAttr(L"WLAN", L"name");
	auto wifi_pwd = xml_mnge->GetNodeAttr(L"WLAN", L"pwd");

	if (wifi_name == L"" || wifi_pwd == L"") {
		MessageBox(m_hWnd, L"无线网络配置有误，请联系管理员", L"Warning", MB_OK);
		return false;
	}

	for (auto item : nics)
		if (nic_name == item.second)
			return net_tool_->ConnectWifi(item.first, wifi_name, wifi_pwd);

	return false;
}

bool MainWnd::InitLocaIP(LPCTSTR nic_name)
{
	auto xml_mnge = App::GetInstance()->GetXmlMnge();
	auto auto_ip = xml_mnge->GetNodeAttrToBool(L"LocalIP", L"auto_ip");
	if (auto_ip) {
		net_tool_->AutoIpForAdapter(nic_name);
		static_cast<CTabLayoutUI*>(m_pm.FindControl(L"Log_tab"))->SelectItem(2);
		// 两秒钟的等待，等IP设置完成后，再进行IP选择
		// 由于是自动获取IP，当有多张网卡的时候可能会有多个IP，所以这里需要用户手动选择启用哪个IP！
		SetTimer(m_hWnd, 221, 3000, NULL);
	}
	else {
		auto ip = xml_mnge->GetNodeAttr(L"LocalIP", L"ip");
		auto gateway = xml_mnge->GetNodeAttr(L"LocalIP", L"gateway");
		auto dns = xml_mnge->GetNodeAttr(L"LocalIP", L"dns");

		if (net_tool_->SetIpForAdapter(nic_name, ip, gateway))
			return net_tool_->SetDnsForAdapter(nic_name, dns);
	}

	return false;
}

bool MainWnd::OnConnBtn()
{
	CComboBoxUI *wlan_list = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("wlan_list")));
	if (!wlan_list)
		return false;

	CDuiString nic_name = wlan_list->GetText();
	if (nic_name == L"")
		return false;

	if (!ConneWifiForNic(nic_name))
		return false;

	if (!InitLocaIP(nic_name))
		return false;

	auto xml_mnge = App::GetInstance()->GetXmlMnge();
	std::string loca_ip = CW2A(xml_mnge->GetNodeAttr(L"LocalIP", L"ip"));

	if (InitLocalClient(loca_ip)) {		// 如果都连接成功了，则初始化学生端，准备登录
		static_cast<CTabLayoutUI*>(m_pm.FindControl(L"Log_tab"))->SelectItem(0);
		return true;
	}

	return false;
}

bool MainWnd::OnChioceIp()
{
	auto ip_list = static_cast<CComboBoxUI*>(m_pm.FindControl(_T("ip_list")));
	if (!ip_list)
		return false;

	std::string loca_ip = CW2A(ip_list->GetText());
	if (loca_ip == "")
		return false;

	if (InitLocalClient(loca_ip)) {
		static_cast<CTabLayoutUI*>(m_pm.FindControl(L"Log_tab"))->SelectItem(0);
		return true;
	}
	return false;
}

bool MainWnd::OnSetWLANInfo()
{
	auto app_id = m_pm.FindControl(L"app_id")->GetText();
	auto wifi_name = m_pm.FindControl(L"wifi_name")->GetText();
	auto wifi_pad = m_pm.FindControl(L"wifi_pwd")->GetText();
	if (app_id == L"" || wifi_name == L"" || wifi_pad == L"") {
		MessageBox(m_hWnd, L"信息不合理", L"ERROR", MB_OK);
		return false;
	}

	auto xml_mnge = App::GetInstance()->GetXmlMnge();
	xml_mnge->SetNodeAttr(L"AppID", L"value", app_id);
	xml_mnge->SetNodeAttr(L"WLAN", L"name", wifi_name.GetData());
	xml_mnge->SetNodeAttr(L"WLAN", L"pwd", wifi_pad.GetData());

	auto auto_ip_check = static_cast<CCheckBoxUI*>(m_pm.FindControl(L"auto_ip_check"));
	auto selected = auto_ip_check->IsSelected();
	xml_mnge->SetNodeAttr(L"LocalIP", L"auto_ip", selected);
	if (!selected) {
		auto ip_addr = static_cast<IpControlUI*>(m_pm.FindControl(L"ip_addr"));
		auto gateway_addr = static_cast<IpControlUI*>(m_pm.FindControl(L"gateway_addr"));
		auto dns_addr = static_cast<IpControlUI*>(m_pm.FindControl(L"dns_addr"));

		if(!ip_addr->IsReasonable() || 
			!gateway_addr->IsReasonable() || 
			!dns_addr->IsReasonable()) {
			MessageBox(m_hWnd, L"信息不合理", L"ERROR", MB_OK);
			return false;
		}
		xml_mnge->SetNodeAttr(L"LocalIP", L"ip", ip_addr->GetText());
		xml_mnge->SetNodeAttr(L"LocalIP", L"gateway", gateway_addr->GetText());
		xml_mnge->SetNodeAttr(L"LocalIP", L"dns", dns_addr->GetText());
	}

	auto svip_addr = static_cast<IpControlUI*>(m_pm.FindControl(L"svip_addr"));
	auto sv_port = m_pm.FindControl(L"port")->GetText();
	if (!svip_addr->IsReasonable() || sv_port == L"") {
		MessageBox(m_hWnd, L"信息不合理", L"ERROR", MB_OK);
		return false;
	}
	xml_mnge->SetNodeAttr(L"ServerIp", L"value", svip_addr->GetText());
	xml_mnge->SetNodeAttr(L"ServerPort", L"value", sv_port);

	static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("anima_tab")))->SelectItem(0);
	return true;
}
