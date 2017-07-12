#include "stdafx.h"
#include "net_tool.h"
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <NetCon.h>
#include <locale.h>
#include <wlanapi.h>
#include <sstream>

#pragma comment(lib,"Iphlpapi.lib")

NetworkTool::NetworkTool()
	: hClient_(nullptr)
{
}

NetworkTool::~NetworkTool()
{
	if (hClient_)
		WlanCloseHandle(hClient_, nullptr);
}

bool NetworkTool::NetworkInit()
{
	return false;
}

bool NetworkTool::AutoIpAll()
{
	std::vector<std::pair<GUID, std::wstring>> nics;
	if (!GetAllNICsVector(nics))
		return false;

	for (auto item : nics)
		AutoIpForAdapter(item.second.c_str());

	return true;
}

bool NetworkTool::AutoIpForAdapter(LPCTSTR adpt_name)
{
	auto set_ip = [&](LPCTSTR type) ->bool {
		TCHAR comline[MAX_PATH] = { 0 };
		LPCTSTR format = L"/c netsh interface ip set %s name=\"%s\" dhcp";
		_stprintf_s(comline, format, type, adpt_name);

		return DoShellExe(comline);
	};

	if (set_ip(_T("address")))
		return set_ip(_T("dnsservers"));

	return false;
}

bool NetworkTool::GetAllNICsVector(std::vector<std::pair<GUID, std::wstring>>& nics)
{
	INetConnectionManager *net_manager = nullptr;
	INetConnection *net_conn = nullptr;
	IEnumNetConnection *net_enum = nullptr;
	ULONG celt_fetched;

	setlocale(LC_CTYPE, "");
	CoInitialize(NULL);
	CoCreateInstance(CLSID_ConnectionManager, NULL, CLSCTX_SERVER, IID_INetConnectionManager, (void**)&net_manager);
	if (net_manager == nullptr) {
		MessageBox(nullptr, _T("������ʧ�ܣ�"), _T("ERROR"), MB_OK);
		return false;
	}

	net_manager->EnumConnections(NCME_DEFAULT, &net_enum);
	NETCON_PROPERTIES *net_proper;
	while (net_enum->Next(1, &net_conn, &celt_fetched) == S_OK) {
		net_conn->GetProperties(&net_proper);
		nics.push_back({ net_proper->guidId, net_proper->pszwName });
	}

	return true;
}

bool NetworkTool::SetIpForAdapter(LPCTSTR adpt_name, LPCTSTR ip, LPCTSTR gateway)
{
	TCHAR comline[MAX_PATH] = { 0 };
	LPCTSTR format = L"/c netsh interface ip set address name=\"%s\" static %s 255.0.0.0 %s";
	_stprintf_s(comline, format, adpt_name, ip, gateway);

	return DoShellExe(comline);
}

bool NetworkTool::SetDnsForAdapter(LPCTSTR adpt_name, LPCTSTR dns)
{
	TCHAR comline[MAX_PATH] = { 0 };
	LPCTSTR format = L"/c netsh interface ip set dnsservers name=\"%s\" static %s";
	_stprintf_s(comline, format, adpt_name, dns);

	return DoShellExe(comline);
}

bool NetworkTool::GetWlanNICVector(std::vector<std::pair<GUID, std::wstring>>& nics)
{
	DWORD dwResult = 0;
	if (!hClient_) {
		DWORD dwMaxClien = 2;
		DWORD dwCurVersion = 0;
		if (WlanOpenHandle(dwMaxClien, NULL, &dwCurVersion, &hClient_) != ERROR_SUCCESS) {
			OutputDebugString(L"\n - - - WlanOpenHandle failed with error! - - -\n");
			if (hClient_)
				WlanCloseHandle(hClient_, nullptr);
			return false;
		}
	}

	PWLAN_INTERFACE_INFO_LIST pIfList = nullptr;
	if (WlanEnumInterfaces(hClient_, nullptr, &pIfList) != ERROR_SUCCESS) {
		OutputDebugString(L"\n - - - WlanEnumInterfaces failed with error! - - -\n");
		return false;
	}

	std::vector<std::pair<GUID, std::wstring>> all_nic;
	if (!GetAllNICsVector(all_nic))
		return false;

	PWLAN_INTERFACE_INFO pIfInfo = nullptr;
	for (int index_card = 0; index_card < pIfList->dwNumberOfItems; ++index_card) {
		pIfInfo = &pIfList->InterfaceInfo[index_card];
		for (auto item : all_nic) {
			if (item.first == pIfInfo->InterfaceGuid) {
				nics.push_back(item);
				break;
			}
		}
	}

	return true;
}

bool NetworkTool::ConnectWifi(GUID guid, LPCTSTR wifi_name, LPCTSTR wifi_pwd)
{
	if (!hClient_) {
		DWORD dwMaxClien = 2;
		DWORD dwCurVersion = 0;
		if (WlanOpenHandle(dwMaxClien, NULL, &dwCurVersion, &hClient_) != ERROR_SUCCESS) {
			OutputDebugString(L"\n - - - WlanOpenHandle failed with error! - - -\n");
			if (hClient_)
				WlanCloseHandle(hClient_, nullptr);
			hClient_ = nullptr;
			return false;
		}
	}

	DWORD dwFlags;
	DWORD pdwGrantedAccess;
	LPWSTR xmlfile;
	if (WlanGetProfile(hClient_, &guid, wifi_name, nullptr, &xmlfile, &dwFlags, &pdwGrantedAccess) != ERROR_SUCCESS) {
		LPCWSTR profileXml;
		std::wstring strHead0 = L"<?xml version=\"1.0\"?>\
				<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name>";
		std::wstring strHead1 = L"</name><SSIDConfig><SSID><name>";
		std::wstring strHead2 = L"</name>\
				</SSID>\
				</SSIDConfig>\
				<connectionType>ESS</connectionType>\
				<connectionMode>auto</connectionMode>\
				<MSM>\
				<security>\
				<authEncryption>\
				<authentication>WPA2PSK</authentication>\
				<encryption>AES</encryption>\
				<useOneX>false</useOneX>\
				</authEncryption>\
				<sharedKey>\
				<keyType>passPhrase</keyType>\
				<protected>false</protected>\
				<keyMaterial>";
		std::wstring strHead3 = L"</keyMaterial>\
				</sharedKey>\
				</security>\
				</MSM>\
				</WLANProfile>";
		strHead0 = strHead0 + wifi_name + strHead1 + wifi_name + strHead2 + wifi_pwd + strHead3;

		profileXml = strHead0.c_str();
		dwFlags = 0;
		WLAN_REASON_CODE dwReasonCode;
		if (WlanSetProfile(hClient_, &guid, dwFlags, profileXml, nullptr, true, nullptr, &dwReasonCode) != ERROR_SUCCESS) {
			OutputDebugString(L"\n - - - ���������ļ�ʧ�ܣ� - - -\n");
			return false;
		}
	}

	WLAN_CONNECTION_PARAMETERS connect_param;
	connect_param.wlanConnectionMode = wlan_connection_mode_profile;
	connect_param.strProfile = wifi_name;
	connect_param.pDot11Ssid = nullptr;
	connect_param.dot11BssType = dot11_BSS_type_infrastructure;
	connect_param.pDesiredBssidList = nullptr;
	connect_param.dwFlags = WLAN_CONNECTION_HIDDEN_NETWORK;

	if (WlanConnect(hClient_, &guid, &connect_param, nullptr) == ERROR_SUCCESS) {
		OutputDebugString(L"\n- - - - - ���ӳɹ��� - - - - -\n");
		return true;
	}
	else {
		OutputDebugString(L"\n- - - - - ����ʧ�ܣ� - - - - -\n");
		return false;
	}
	return false;
}

bool NetworkTool::GetAllLocalIP(std::vector<std::wstring>& ips)
{
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	// ���stSize ����������Ҳ���������
	if (ERROR_BUFFER_OVERFLOW == nRel) {
		// ERROR_BUFFER_OVERFLOW ��ʾ ���ݸ� GetAdaptersInfo ���ڴ�ռ䲻����ͬʱ������ stSize ��ʾ����Ŀռ��С
		delete pIpAdapterInfo;	// �ͷŵ������·��䣡
		pIpAdapterInfo = (PIP_ADAPTER_INFO) new BYTE[stSize];
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	// ���ô������¿ռ��Сֵ��������� pIpAdapterInfo
	}
	
	if (ERROR_SUCCESS == nRel) {
		std::wostringstream oss;
		while (pIpAdapterInfo) {
			auto iter = &pIpAdapterInfo->IpAddressList;
			while (iter) {
				oss.str(L"");
				oss << iter->IpAddress.String;
				if (oss.str() != L"0.0.0.0")		// ���β������IP��ַ
					ips.push_back(oss.str());
				iter = iter->Next;
			}
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
		delete pIpAdapterInfo;	// �ͷŵ�
		return true;
	}
	return false;
}

bool NetworkTool::DoShellExe(LPCTSTR comline)
{
	SHELLEXECUTEINFO shell_info = { 0 };
	shell_info.cbSize = sizeof(SHELLEXECUTEINFO);
	shell_info.fMask = SEE_MASK_NOCLOSEPROCESS;
	shell_info.lpVerb = _T("runas");      // runas ����ʵ���Թ���ԱȨ�����г���
	shell_info.hwnd = nullptr;
	shell_info.lpFile = _T("cmd.exe");
	shell_info.lpDirectory = nullptr;
	shell_info.nShow = SW_HIDE;
	shell_info.lpParameters = comline;
	shell_info.hInstApp = nullptr;

	DWORD exitcode;
	ShellExecuteEx(&shell_info);
	return GetExitCodeProcess(shell_info.hProcess, &exitcode);
}
