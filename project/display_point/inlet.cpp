#include "application.h"

//程序入口处调用该函数进行内存泄露检测
//程序退出前调用_CrtDumpMemoryLeaks();函数可查看内存泄露的ID号，该ID号可作为Breakpoint的参数，运行时将会在相应位置自动断点
inline  void  EnableMemLeakCheck(int Breakpoint = 0) {
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	if (Breakpoint)
		_CrtSetBreakAlloc(1912);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	EnableMemLeakCheck();
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
		return 0;

	App *app = App::GetInstance();
	if (app) {
		if (app->Initialize(hInstance))
			app->Run();
		app->DestroyInstance();
		app = nullptr;
	}

	::CoUninitialize();
	_CrtDumpMemoryLeaks();
	return 0;
}