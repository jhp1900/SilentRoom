#include "setup_wnd.h"
#include "msg_head.h"
#include "xml_manager.h"

//#include "..\student_client\application.h"
#include "..\display_point\application.h"

SetupWnd::SetupWnd(HWND pa_hwnd)
	: pa_hwnd_(pa_hwnd)
{
}

SetupWnd::~SetupWnd()
{
}

bool SetupWnd::DoModal(HWND hwnd)
{
	if (!Create(hwnd, _T(""), WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_ACCEPTFILES))
		return false;
	CenterWindow();
	return ShowModal() != 0;
}

void SetupWnd::InitWindow()
{
	auto xml_mnge = App::GetInstance()->GetXmlMnge();
	LPCTSTR ip = xml_mnge->GetNodeAttr(_T("ServerIp"), _T("value"));
	LPCTSTR port = xml_mnge->GetNodeAttr(_T("ServerPort"), _T("value"));
	m_pm.FindControl(_T("ip_address"))->SetText(ip);
	m_pm.FindControl(_T("port"))->SetText(port);
}

void SetupWnd::OnClickBtn(TNotifyUI & msg, bool & handled)
{
	if (msg.pSender->GetName() == _T("ok_btn")) {
		IpControlUI *ip_ctrl = static_cast<IpControlUI*>(m_pm.FindControl(_T("ip_address")));
		auto server_port = m_pm.FindControl(_T("port"));
		if (ip_ctrl->IsReasonable() && server_port->GetText() != _T("")) {
			LPCTSTR ip = ip_ctrl->GetText();
			LPCTSTR port = server_port->GetText();
			auto xml_mnge = App::GetInstance()->GetXmlMnge();
			xml_mnge->SetNodeAttr(_T("ServerIp"), _T("value"), ip);
			xml_mnge->SetNodeAttr(_T("ServerPort"), _T("value"), port);
			::PostMessage(pa_hwnd_, kAM_IPSetupMsg, 0, 0);			// ������Ϣ��֪ͨ ������IP ���������
		} else {
			MessageBox(m_hWnd, _T("��Ϣ������!"), _T("Message"), MB_OK);
			return;
		}
	}

	Close(0);
}