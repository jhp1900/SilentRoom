#include "main_wnd.h"

MainWnd::MainWnd()
{
}

MainWnd::~MainWnd()
{
}

void MainWnd::InitWindow()
{
	int x = GetSystemMetrics(SM_CXSCREEN) - 110;
	::SetWindowPos(*this, NULL, x, 50, 60, 60, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

