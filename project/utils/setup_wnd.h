#pragma once
#include "stdafx.h"
#include "ip_control_ui.h"

class SetupWnd : public WindowImplBase
{
public:
	SetupWnd(HWND pa_hwnd);
	~SetupWnd();

	DECLARE_DUIWND_INFO(_T("SetupWnd"), CS_DBLCLKS, _T("setup_wnd.xml"))

	BEGIN_DUICONTROL_CREATE(SetupWnd)
		DUICONTROL_CREATE_FROM_XML(_T("IpControl"), _T("ip_control.xml"))
		DUICONTROL_CREATE(_T("IpControlLayout"), IpControlUI)
	END_DUICONTROL_CREATE()

	BEGIN_DUINOTIFY_MAP(SetupWnd)
		DUINOTIFY_TYPE_HANDLER(DUI_MSGTYPE_CLICK, OnClickBtn)
	END_DUINOTIFY_MAP()

public:
	bool DoModal(HWND hwnd);
	virtual void InitWindow() override;

private:
	void OnClickBtn(TNotifyUI& msg, bool& handled);

private:
	HWND pa_hwnd_;
};

