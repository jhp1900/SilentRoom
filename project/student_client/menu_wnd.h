#pragma once
#include "stdafx.h"
#include "msg_head.h"

enum MenuMsg
{
	MenuMsgClose,		// ÍË³öÏûÏ¢
	MenuMsgSpeak,
	MenuMsgHandup,
	MenuMsgStop,
	MenuMsgSetup,
};

class MenuWnd : public WindowImplBase
{
public:
	MenuWnd(HWND pa_hwnd);
	~MenuWnd();

	DECLARE_DUIWND_INFO(_T("MenuWnd"), CS_DBLCLKS, _T("sc_menu_wnd.xml"))

	BEGIN_DUIMSG_MAP(MenuWnd)
		DUIMSG_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		DUIMSG_HANDLER(WM_KEYDOWN, OnKeyDown)
		DUIMSG_HANDLER(WM_KILLFOCUS, OnKillFocus)
	END_DUIMSG_MAP()

	BEGIN_DUINOTIFY_MAP(MenuWnd)
		DUINOTIFY_TYPE_HANDLER(DUI_MSGTYPE_CLICK, OnClickBtn)
	END_DUINOTIFY_MAP()

public:
	void InitWindow() override;
	HWND CreateWithStyle(DWORD style);
	HWND CreateWithDefaltStyle();

private:
	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) override;
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam) override;

private:
	void OnClickBtn(TNotifyUI& msg, bool& handled);

public:
	void PopupWnd(PPOINT point, bool left_bottom = false);

private:
	HWND pa_hwnd_;
};

