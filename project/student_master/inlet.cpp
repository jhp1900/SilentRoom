#include "application.h"

//������ڴ����øú��������ڴ�й¶���
//�����˳�ǰ����_CrtDumpMemoryLeaks();�����ɲ鿴�ڴ�й¶��ID�ţ���ID�ſ���ΪBreakpoint�Ĳ���������ʱ��������Ӧλ���Զ��ϵ�
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