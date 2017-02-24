#pragma once
#include "stdafx.h"
#include "application.h"

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
		DUIMSG_HANDLER(Silent_Handup, OnHandup)
		//DUIMSG_HANDLER(kAM_ResetIPInfo, OnResetIPInfoMsg)
		//DUIMSG_HANDLER(kAM_DPISetMsg, OnDPISetMsg)
	END_DUIMSG_MAP()

public:
	virtual void InitWindow() override;

	LRESULT OnHandup(UINT uMsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled);
private:
};
