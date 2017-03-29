#include "main_wnd.h"
#include "mst_setup_wnd.h"

MainWnd::MainWnd()
{
}

MainWnd::~MainWnd()
{
	int a = 0;
}

LRESULT MainWnd::OnHandup(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	WebServer* web_server = App::GetInstance()->GetWebServer();
	if (MessageBox(NULL, L"FFFF", L"HANDUP", MB_OKCANCEL) == IDOK)
		web_server->SetStreamIp((char*)wparam);

	return LRESULT();
}

void MainWnd::OnClickBtn(TNotifyUI &msg, bool &handled)
{
	//MstSetupWnd mst_setup;
	//mst_setup.DoModal(m_hWnd);
	::PostQuitMessage(0);
}

void MainWnd::InitWindow()
{
	/* ÉèÖÃÍøÒ³Ö÷Ò³ */
	auto web_browser = static_cast<CWebBrowserUI*>(m_pm.FindControl(_T("web_browser")));
	web_browser->SetHomePage(CPaintManagerUI::GetResourcePath() + _T("html/students.html"));
}

