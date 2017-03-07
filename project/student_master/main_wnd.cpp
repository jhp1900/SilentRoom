#include "main_wnd.h"

MainWnd::MainWnd()
{
}

MainWnd::~MainWnd()
{
}

LRESULT MainWnd::OnHandup(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	WebServer* web_server = App::GetInstance()->GetWebServer();
	if (MessageBox(NULL, L"FFFF", L"HANDUP", MB_OKCANCEL) == IDOK)
		web_server->SetStreamIp((char*)wparam);

	return LRESULT();
}

void MainWnd::InitWindow()
{
}

