#include "menu_wnd.h"

MenuWnd::MenuWnd(HWND pa_hwnd)
	: pa_hwnd_(pa_hwnd)
{
}

MenuWnd::~MenuWnd()
{
}

void MenuWnd::InitWindow()
{
}

HWND MenuWnd::CreateWithStyle(DWORD style)
{
	return Create(pa_hwnd_, nullptr, style, 0);
}

HWND MenuWnd::CreateWithDefaltStyle()
{
	return Create(pa_hwnd_, nullptr,
		WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
}

LRESULT MenuWnd::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	return LRESULT();
}

LRESULT MenuWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	return LRESULT();
}

LRESULT MenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	ShowWindow(SW_HIDE);
	bHandled = FALSE;
	return LRESULT();
}

LRESULT MenuWnd::ResponseDefaultKeyEvent(WPARAM wParam)
{
	ShowWindow(SW_HIDE);
	return LRESULT();
}

void MenuWnd::OnClickBtn(TNotifyUI & msg, bool & handled)
{
	CDuiString name = msg.pSender->GetName();
	if (name == _T("test1"))
		::SendMessage(pa_hwnd_, kAM_TrayMenuMsg, MenuMsgTemp1, 0);
	else if (name == _T("test2"))
		::SendMessage(pa_hwnd_, kAM_TrayMenuMsg, MenuMsgTemp2, 0);
	else if (name == _T("stop"))
		::SendMessage(pa_hwnd_, kAM_TrayMenuMsg, MenuMsgStop, 0);
	else if (name == _T("setup_item"))
		::SendMessage(pa_hwnd_, kAM_TrayMenuMsg, MenuMsgSetup, 0);
	else if (name == _T("closebtn"))
		::SendMessage(pa_hwnd_, kAM_TrayMenuMsg, MenuMsgClose, 0);

	ShowWindow(SW_HIDE);
}

void MenuWnd::PopupWnd(PPOINT point, bool left_bottom)
{
	int srceen_width = ::GetSystemMetrics(SM_CXSCREEN);
	int srceen_height = ::GetSystemMetrics(SM_CYSCREEN);
	int x = point->x;
	int y = point->y;

	RECT rect = { 0 };
	GetWindowRect(m_hWnd, &rect);
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;

	if (x + w > srceen_width)
		x -= w;
	if (y + h > srceen_height)
		y -= h;

	::SetWindowPos(m_hWnd, 0, x, y, w, h, SWP_NOZORDER | SWP_NOSIZE);
	ShowWindow();
}
