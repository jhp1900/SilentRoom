#pragma once
#include "stdafx.h"

class MainWnd : public WindowImplBase
{
public:
	MainWnd();
	~MainWnd();

	DECLARE_DUIWND_INFO(_T("MainWnd"), CS_DBLCLKS, _T("display_main_wnd.xml"))

	BEGIN_DUIMSG_MAP(MainWnd)
		//DUIMSG_HANDLER(kAM_InitOtherWndMsg, OnInitOtherWndMsg)
		//DUIMSG_HANDLER(kAM_ResetIPInfo, OnResetIPInfoMsg)
		//DUIMSG_HANDLER(kAM_DPISetMsg, OnDPISetMsg)
	END_DUIMSG_MAP()

public:
	virtual void InitWindow() override;

private:
};
