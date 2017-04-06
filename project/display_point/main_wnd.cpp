#include "main_wnd.h"
#include "resource.h"
#include "application.h"
#include "vlc_tool.h"
#include "..\utils\json_operate.h"
#include <atlbase.h>
#include "..\utils\setup_wnd.h"
#include "..\utils\xml_manager.h"
#include <math.h>
#include "..\utils\utils.h"


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
	, local_ip_("")
	, show_wnd_(false)
	, tray_menu_(nullptr)
	, Video_wnd_(nullptr)
	, rpc_server_(nullptr)
	, rpc_listen_thread_(nullptr)
	, web_client_(nullptr)
	, alpha_(255)
	, play_hwnd_(NULL)
	, speaker_("")
{
}

MainWnd::~MainWnd()
{
	Shell_NotifyIcon(NIM_DELETE, &tray_data_);
}

void MainWnd::InitWindow()
{
	AddTray();		// �������
	GetLocalIP(local_ip_);	// ��ȡ����IP, ���� ivga �㲥
	Animation();	// ������Ч

	web_client_.reset(new WebStudentClient);

	// ���������Ŵ���
	Video_wnd_.reset(new VideoWnd);
	play_hwnd_ = Video_wnd_->Init(m_hWnd);

	// ��ʼ�� RPC ������������ RPC �����߳�
	StartRpcThread();

	// ��ʼ�������� ivga �㲥
	if (!App::GetInstance()->GetVLCTool()->BeginBroadcast(local_ip_))
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
		case MenuMsgSetup:
			SetupWnd setup_wnd(m_hWnd);
			setup_wnd.DoModal(m_hWnd);
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
			SetTimer(m_hWnd, 5, 5, nullptr);
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

	auto ip_check = [&]() {
		KillTimer(m_hWnd, 5);

		auto xml_mnge = App::GetInstance()->GetXmlMnge();
		std::string server_ip = CW2A(xml_mnge->GetNodeAttr(_T("ServerIp"), _T("value")));
		std::string server_port = CW2A(xml_mnge->GetNodeAttr(_T("ServerPort"), _T("value")));
		if (server_ip == "" || server_port == "") {
			if (MessageBox(m_hWnd, _T("��δ���÷�����IP���Ƿ�������ã�"), _T("Message"), MB_YESNO) == IDYES) {
				SetupWnd setup_wnd(m_hWnd);
				setup_wnd.DoModal(m_hWnd);
			} else {
				m_pm.FindControl(_T("point_label"))->SetText(_T("�������� ������IP��"));
				SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA);
				::ShowWindow(m_hWnd, SW_SHOW);
				return;
			}
		} else {
			web_client_->Initial(server_ip + ":" + server_port);	// ���� Web �ͻ���
			::PostMessage(m_hWnd, kAM_BeginKeepaLive, 0, 0);
		}
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
		case 5:
			ip_check();
			break;
	}
	
	return LRESULT();
}

//RPCͨ�ŷ�ʽ
LRESULT MainWnd::OnRpcHandupMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	std::string json_str = (char*)wparam;

	JsonOperate json_operate;
	StudentData student_data;
	json_operate.JsonAnalysis(json_str.c_str(), student_data);
	
	if (student_data.operate_type_ == OperateType::SPEAK) {
		if (speaker_ != "") 						// ���������ڷ���ʱ�����µķ�����Ϣ�������� 
			return 0;
		if (PlayStream(student_data.stream_ip_, student_data.naem_)) {
			web_client_->SendWebMessage(json_str);	// ֪ͨ�������ı�״̬
		}
	} else if (student_data.operate_type_ == OperateType::STOP_SPEAK) {
		if (speaker_ == student_data.naem_) {		// �����˺͵�ǰ�����˱�����ͬ������Ч�� ���������Ƿ������Լ��������� 
			StopStream();
			speaker_ = "";							// �������Ժ󣬷��������ÿ�
			web_client_->SendWebMessage(json_str);
		}
	} else if (student_data.operate_type_ == OperateType::HANDUP) {
		web_client_->SendWebMessage(json_str);		// ����Ǿ��ַ��ԣ���ʾҪ������ʦͬ�⣩�������Ƿ����˷��ԣ����������ת����Ϣ����Ϊ��ʦ�в���Ȩ
	}

	return LRESULT();
}

LRESULT MainWnd::OnIpSetupMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	//std::string server_ip = CW2A(App::GetInstance()->GetXmlMnge()->GetNodeAttr(_T("ServerIp"), _T("value")));
	//web_client_->Initial(server_ip);
	m_pm.FindControl(_T("point_label"))->SetText(_T("��ӭʹ��***!"));
	Animation();	// ������Ч
	return LRESULT();
}

//WEBͨ�ŷ�ʽ
LRESULT MainWnd::OnWebRetMsg(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	std::string ret_data = (char*)(wparam);
	JsonOperate json_operate;
	StudentData student_data;
	json_operate.JsonAnalysis(ret_data.c_str(), student_data);
	memset((char*)wparam, 0, ret_data.length());
	if (student_data.operate_type_ == OperateType::TEACHER_CONTROL) {
		if (student_data.naem_ != "") {
			PlayStream(student_data.stream_ip_, student_data.naem_);
		} else {							// ��ʦ�ٿ�ʱ���� name_ Ϊ�գ��������������
			StopStream();
			speaker_ = "";
		}
	} else if (student_data.operate_type_ == OperateType::KEEPA_LIVE) {
		if (student_data == last_sutdentdata_) {
			return LRESULT();
		} else {
			last_sutdentdata_ = student_data;	//�����ϴβ������������ظ�����ͬһ��

			if (student_data.handup_) {
				PlayStream(student_data.stream_ip_, student_data.naem_);
			} else {
				StopStream();					//���������ж�
			}
		}
	}

	return LRESULT();
}

LRESULT MainWnd::OnPlayStream(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	//VLCTool *vlc = App::GetInstance()->GetVLCTool();
	//vlc->DestoryPlay();
	//if (!vlc->PlayStream(play_hwnd_, ((std::string*)wparam)->c_str())) {
	//	vlc->DestoryPlay();
	//	return false;
	//}
	//m_pm.FindControl(_T("point_label"))->SetText(_T("teacher"));
	//::ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	//show_wnd_ = true;
	//SetTimer(m_hWnd, 3, 6300, nullptr);			// ��ʱ�� 3�� ���ڵȴ��������ȶ��Լ���ʾ��������Դ

	return LRESULT();
}

LRESULT MainWnd::OnBeginKeepaLive(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	web_client_->SendWebMessage("fdsa", true);
	return LRESULT();
}

void MainWnd::AddTray()
{
	// ��������ʼ�� ���̲˵�����
	tray_menu_.reset(new MenuWnd(m_hWnd));
	tray_menu_->CreateWithDefaltStyle();

	// ��ʼ�����̽ṹ��
	tray_data_.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	tray_data_.hWnd = this->m_hWnd;
	tray_data_.uID = 1;
	tray_data_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	tray_data_.uCallbackMessage = kAM_TrayCallbackMsg;
	tray_data_.hIcon = LoadIcon((HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDI_ICON1));
	wcscpy_s(tray_data_.szTip, L"iVGA");

	Shell_NotifyIcon(NIM_ADD, &tray_data_);		// �������
}

void MainWnd::Animation()
{
	::ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	// ����͸��ɫ  		
	COLORREF cr_key = RGB(0, 0, 0);
	SetLayeredWindowAttributes(m_hWnd, cr_key, 0, LWA_COLORKEY);
	SetTimer(m_hWnd, 1, 3000, nullptr);
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

bool MainWnd::PlayStream(const string &stream_ip, const string &msg_str)
{
	std::string url;

	if (msg_str == "teacher_speak") {
		auto xml_mnge = App::GetInstance()->GetXmlMnge();
		std::string server_ip = CW2A(xml_mnge->GetNodeAttr(_T("ServerIp"), _T("value")));  //��ʦ������ҵ��������ַ
		url = "rtsp://" + server_ip + ":554/session0";
	} else {
		url = "rtsp://" + stream_ip + ":554/live"; //����������ַ
	}

	VLCTool *vlc = App::GetInstance()->GetVLCTool();
	vlc->DestoryPlay();
	if (!vlc->PlayStream(play_hwnd_, url)) {
		vlc->DestoryPlay();
		return false;
	}
	speaker_ = msg_str;
	m_pm.FindControl(_T("point_label"))->SetText(CA2W(msg_str.c_str()));
	::ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	show_wnd_ = true;
	SetTimer(m_hWnd, 3, 6300, nullptr);			// ��ʱ�� 3�� ���ڵȴ��������ȶ��Լ���ʾ��������Դ
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
