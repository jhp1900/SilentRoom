#pragma once
#include "stdafx.h"
#include <memory>

class MainWnd;
class WebServer;

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
	WebServer *GetWebServer() { return web_server_.get(); }

private:
	void InitResource(HINSTANCE inst);

private:
	static App *instance_;
	std::unique_ptr<MainWnd> main_wnd_;
	std::unique_ptr<WebServer> web_server_;
};

