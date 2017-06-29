#include "stdafx.h"
#include "utils.h"
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <sstream>
#include <NetCon.h>

#pragma comment(lib,"Iphlpapi.lib")

void GetLocalIP(std::vector<std::wstring> &ips)
{
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	// 这里，stSize 既是输入量也是输出量；
	if (ERROR_BUFFER_OVERFLOW == nRel) {
		// ERROR_BUFFER_OVERFLOW 表示 传递给 GetAdaptersInfo 的内存空间不够，同时传出的 stSize 表示所需的空间大小
		delete pIpAdapterInfo;	// 释放掉，重新分配！
		pIpAdapterInfo = (PIP_ADAPTER_INFO) new BYTE[stSize];
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	// 利用传出的新空间大小值，重新填充 pIpAdapterInfo
	}
	if (ERROR_SUCCESS == nRel) {
		std::wostringstream oss;
		while (pIpAdapterInfo) {
			auto iter = &pIpAdapterInfo->IpAddressList;
			while (iter) {
				oss.str(L"");
				oss << iter->IpAddress.String;
				ips.push_back(oss.str());
				iter = iter->Next;
			}
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
		delete pIpAdapterInfo;	// 释放掉
	}
}

bool GetNICInfo(std::vector<std::pair<GUID, std::wstring>>& nic_info)
{
	INetConnectionManager *net_manager = nullptr;
	INetConnection *net_conn = nullptr;
	IEnumNetConnection *net_enum = nullptr;
	ULONG celt_fetched;

	setlocale(LC_CTYPE, "");
	CoInitialize(NULL);
	CoCreateInstance(CLSID_ConnectionManager, NULL, CLSCTX_SERVER, IID_INetConnectionManager, (void**)&net_manager);
	if (net_manager == nullptr) {
		MessageBox(nullptr, _T("网络检测失败！"), _T("ERROR"), MB_OK);
		return false;
	}

	net_manager->EnumConnections(NCME_DEFAULT, &net_enum);
	NETCON_PROPERTIES *net_proper;
	while (net_enum->Next(1, &net_conn, &celt_fetched) == S_OK) {
		net_conn->GetProperties(&net_proper);
		nic_info.push_back({ net_proper->guidId, net_proper->pszwName });
	}
	return true;
}

void SetAutoGetIP()
{
	auto set_ip = [](LPCTSTR type, LPCTSTR net_name) {
		TCHAR comline[MAX_PATH] = { 0 };
		_tcscat_s(comline, MAX_PATH, _T("/c netsh interface ip set "));
		_tcscat_s(comline, MAX_PATH, type);
		_tcscat_s(comline, MAX_PATH, _T(" name=\""));
		_tcscat_s(comline, MAX_PATH, net_name);
		_tcscat_s(comline, MAX_PATH, _T("\" dhcp"));

		SHELLEXECUTEINFO shell_info = { 0 };
		shell_info.cbSize = sizeof(SHELLEXECUTEINFO);
		shell_info.fMask = SEE_MASK_NOCLOSEPROCESS;
		shell_info.lpVerb = _T("runas");      // runas 可以实现以管理员权限运行程序
		shell_info.hwnd = nullptr;
		shell_info.lpFile = _T("cmd.exe");
		shell_info.lpDirectory = nullptr;
		shell_info.nShow = SW_HIDE;
		shell_info.lpParameters = comline;
		shell_info.hInstApp = nullptr;

		ShellExecuteEx(&shell_info);
	};

	INetConnectionManager *net_manager = nullptr;
	IEnumNetConnection *net_enum = nullptr;
	INetConnection *net_conn = nullptr;
	ULONG celt_fetched;

	setlocale(LC_CTYPE, "");
	CoInitialize(NULL);
	CoCreateInstance(CLSID_ConnectionManager, NULL, CLSCTX_SERVER, IID_INetConnectionManager, (void**)&net_manager);
	if (net_manager == nullptr) {
		MessageBox(nullptr, _T("网络检测失败！"), _T("ERROR"), MB_OK);
		return;
	}

	net_manager->EnumConnections(NCME_DEFAULT, &net_enum);
	NETCON_PROPERTIES *net_proper;
	while (net_enum->Next(1, &net_conn, &celt_fetched) == S_OK) {
		net_conn->GetProperties(&net_proper);
		set_ip(_T("address"), net_proper->pszwName);
		set_ip(_T("dnsservers"), net_proper->pszwName);
	}
}

std::string UnicodeToUTF8(const wchar_t * str)
{
	int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	char *utf8_str = (char*)malloc(len + 1);
	memset(utf8_str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, str, -1, utf8_str, len, NULL, NULL);
	std::string ret_utf8 = utf8_str;
	free(utf8_str);
	return ret_utf8;
}

std::string UnicodeToUTF8(const char * str)
{
	wchar_t *wide_str = nullptr;
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	wide_str = (wchar_t*)malloc(sizeof(wchar_t)*len);
	MultiByteToWideChar(CP_ACP, 0, str, -1, wide_str, len);
	std::string ret_utf8 = UnicodeToUTF8(wide_str);
	free(wide_str);
	return ret_utf8;
}

wchar_t * UTF8ToUnicode(const wchar_t * utf8)
{
	wchar_t *wide_str = nullptr;
	int len = WideCharToMultiByte(CP_UTF8, 0, utf8, -1, NULL, 0, NULL, NULL);
	char *multi_str = (char*)malloc(len + 1);
	memset(multi_str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, utf8, -1, multi_str, len, NULL, NULL);
	wide_str = UTF8ToUnicode(multi_str);
	free(multi_str);
	return wide_str;
}

wchar_t * UTF8ToUnicode(const char * utf8)
{
	wchar_t *wide_str = nullptr;
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wide_str = (wchar_t*)malloc(sizeof(wchar_t)*len);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wide_str, len);
	return wide_str;
}
