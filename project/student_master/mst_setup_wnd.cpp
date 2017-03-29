#include "mst_setup_wnd.h"
#include "application.h"

MstSetupWnd::MstSetupWnd()
{
}

MstSetupWnd::~MstSetupWnd()
{
}

bool MstSetupWnd::DoModal(HWND hwnd)
{
	if (!Create(hwnd, _T(""), WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_ACCEPTFILES))
		return false;
	CenterWindow();
	return ShowModal() != 0;
}

void MstSetupWnd::InitWindow()
{
	auto xml_mnge = App::GetInstance()->GetXmlMnge();
}

void MstSetupWnd::OnClickBtn(TNotifyUI & msg, bool & handled)
{
	if (msg.pSender->GetName() == _T("ok")) {

	}
	Close(0);
}
