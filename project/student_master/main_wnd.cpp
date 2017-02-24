#include "main_wnd.h"

MainWnd::MainWnd()
{
}

MainWnd::~MainWnd()
{
}

LRESULT MainWnd::OnHandup(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	App::GetInstance()->GetWebServer();
	MessageBox(NULL, L"FFFF", L"HANDUP", MB_OKCANCEL);
	return LRESULT();
}

void MainWnd::InitWindow()
{
}

