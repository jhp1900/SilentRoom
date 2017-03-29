#pragma once
#include "ip_control_ui.h"

class MstSetupWnd : public WindowImplBase
{
public:
	MstSetupWnd();
	~MstSetupWnd();

	DECLARE_DUIWND_INFO(_T("MstSetupWnd"), CS_DBLCLKS, _T("stum_setup_wnd.xml"))

	BEGIN_DUICONTROL_CREATE(MstSetupWnd)
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
};

