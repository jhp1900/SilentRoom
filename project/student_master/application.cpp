#include "application.h"
#include "main_wnd.h"
#include "web_server.h"
#include "..\utils\utils.h"
#include "..\utils\xml_manager.h"

App *App::instance_ = nullptr;

App::App()
{
	xml_mnge_.reset(new XmlManager());
}

App::~App()
{
}

App * App::GetInstance()
{
	return instance_ ? instance_ : (instance_ = new App());
}

void App::DestroyInstance()
{
	if (instance_) {
		delete instance_;
		instance_ = nullptr;
	}
}

bool App::Initialize(HINSTANCE inst)
{
	InitResource(inst);
	xml_mnge_->LoadFile(CPaintManagerUI::GetResourcePath(), _T("configs.xml"));

	std::string loc_ip("");
	GetLocalIP(loc_ip);

	/* 启动WEB服务器 */
	web_server_.reset(new WebServer());
	web_server_->Initial(1, "18.18.8.127", 8082);
	web_server_->ServerStart();
	return true;
}

int App::Run()
{
	main_wnd_.reset(new MainWnd());
	if (!main_wnd_)
		return -1;

	main_wnd_->CreateDuiWindow(NULL, _T(""), UI_WNDSTYLE_FRAME, 0L);
	if (main_wnd_->GetHWND() == nullptr)
		return -1;
	main_wnd_->CenterWindow();
	::ShowWindow(*main_wnd_, SW_SHOW);

	CPaintManagerUI::MessageLoop();

	return 0;
}

void App::InitResource(HINSTANCE inst)
{
	CPaintManagerUI::SetInstance(inst);

	// 资源类型
	CPaintManagerUI::SetResourceType(UILIB_FILE);
	// 资源路径
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	// 加载资源
	switch (CPaintManagerUI::GetResourceType())
	{
		case UILIB_FILE:
		{
			strResourcePath += _T("..\\skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
		case UILIB_RESOURCE:
		{
			strResourcePath += _T("..\\skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("IDR_RES"), _T("xml"));
			break;
		}
		case UILIB_ZIP:
		{
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			CPaintManagerUI::SetResourceZip(_T("..\\skin.zip"), true);
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
		case UILIB_ZIPRESOURCE:
		{
			strResourcePath += _T("..\\skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());

			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), _T("IDR_ZIPRES"), _T("ZIPRES"));
			if (hResource != NULL) {
				DWORD dwSize = 0;
				HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
				if (hGlobal != NULL) {
					dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
					if (dwSize > 0) {
						CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
						// 加载资源管理器
						CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
					}
				}
				::FreeResource(hResource);
			}
		}
		break;
	}
}
