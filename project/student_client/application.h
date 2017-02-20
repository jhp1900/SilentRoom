#pragma once
#include "stdafx.h"
#include <memory>

class MainWnd;
class VLCTool;
class XmlManager;

const string appid_str = "01";

class App
{
private:
	App();
	~App();

public:
	static App *GetInstance();
	static void DestroyInstance();

	bool Initialize(HINSTANCE inst);
	int Run();

	MainWnd *GetMainWnd() { return main_wnd_.get(); }
	VLCTool *GetVLCTool() { return vlc_tool_.get(); }
	XmlManager *GetXmlMnge() { return xml_mnge_.get(); }

private:
	void InitResource(HINSTANCE inst);

private:
	static App *instance_;
	std::unique_ptr<MainWnd> main_wnd_;
	std::unique_ptr<VLCTool> vlc_tool_;
	std::unique_ptr<XmlManager> xml_mnge_;
	//int app_id_;
};

