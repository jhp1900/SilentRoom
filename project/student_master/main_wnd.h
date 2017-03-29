#pragma once
#include "stdafx.h"
#include "application.h"
#include "web_server.h"
#include "..\utils\msg_head.h"

enum Msg {
	Silent_Handup = WM_APP + 20
};

class MainWnd : public WindowImplBase
{
public:
	MainWnd();
	~MainWnd();

	DECLARE_DUIWND_INFO(_T("MainWnd"), CS_DBLCLKS, _T("stum_main_wnd.xml"))

	BEGIN_DUIMSG_MAP(MainWnd)
		DUIMSG_HANDLER(kAM_Silent_Handup, OnHandup)
	END_DUIMSG_MAP()

	BEGIN_DUINOTIFY_MAP(MainWnd)
		DUINOTIFY_HANDLER(_T("setup"), DUI_MSGTYPE_CLICK, OnClickBtn)
	END_DUINOTIFY_MAP()

public:
	virtual void InitWindow() override;

	LRESULT OnHandup(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
	void OnClickBtn(TNotifyUI &msg, bool &handled);		// µ„ª˜ …Ë÷√∞¥≈•
private:
};
