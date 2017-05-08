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
	track_mouse_event_.cbSize = sizeof(TRACKMOUSEEVENT);
	track_mouse_event_.dwFlags = TME_LEAVE | TME_HOVER;
	track_mouse_event_.hwndTrack = this->m_hWnd;
	track_mouse_event_.dwHoverTime = 10;
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

LRESULT MenuWnd::OnMouseLeaveWnd(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL & bHandled)
{
	//ShowWindow(SW_HIDE);
	return LRESULT();
}

void MenuWnd::OnClickBtn(TNotifyUI & msg, bool & handled)
{
	CDuiString name = msg.pSender->GetName();
	if (name == _T("speak_item"))
		::PostMessage(pa_hwnd_, kAM_TrayMenuMsg, MenuMsgSpeak, 0);
	else if (name == _T("handup_item"))
		::PostMessage(pa_hwnd_, kAM_TrayMenuMsg, MenuMsgHandup, 0);
	else if (name == _T("stop_item"))
		::PostMessage(pa_hwnd_, kAM_TrayMenuMsg, MenuMsgStop, 0);
	else if (name == _T("setup_item"))
		::PostMessage(pa_hwnd_, kAM_TrayMenuMsg, MenuMsgSetup, 0);
	else if (name == _T("closebtn"))
		::PostMessage(pa_hwnd_, kAM_TrayMenuMsg, MenuMsgClose, 0);

	ShowWindow(SW_HIDE);
}

void MenuWnd::PopupWnd(PPOINT point, bool left_bottom)
{
	TrackMouseEvent(&track_mouse_event_);

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
