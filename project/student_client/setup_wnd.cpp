#include "setup_wnd.h"
#include "msg_head.h"
#include "application.h"
#include "xml_manager.h"

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
	LPCTSTR ip = App::GetInstance()->GetXmlMnge()->GetNodeAttr(_T("ServerIp"), _T("value"));
	m_pm.FindControl(_T("ip_address"))->SetText(ip);
}

void SetupWnd::OnClickBtn(TNotifyUI & msg, bool & handled)
{
	if (msg.pSender->GetName() == _T("ok_btn")) {
		IpControlUI *ip_ctrl = static_cast<IpControlUI*>(m_pm.FindControl(_T("ip_address")));
		if (ip_ctrl->IsReasonable()) {
			LPCTSTR ip = ip_ctrl->GetText();
			App::GetInstance()->GetXmlMnge()->SetNodeAttr(_T("ServerIp"), _T("value"), ip);
		} else {
			MessageBox(nullptr, _T("IP信息不合理!"), _T("Message"), MB_OK);
			return;
		}
	}

	Close(0);
}
