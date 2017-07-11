#pragma once
#include "stdafx.h"

class NetworkTool
{
public:
	NetworkTool();
	~NetworkTool();

	bool NetworkInit();

	bool AutoIpAll();
	bool AutoIpForAdapter(LPCTSTR adpt_name);
	bool GetAllNICsVector(std::vector<std::pair<GUID, std::wstring>>& nics);
	bool SetIpForAdapter(LPCTSTR adpt_name, LPCTSTR ip, LPCTSTR gateway);
	bool SetDnsForAdapter(LPCTSTR adpt_name, LPCTSTR dns);

	bool GetWlanNICVector(std::vector<std::pair<GUID, std::wstring>>& nics);
	bool ConnectWifi(GUID guid, LPCTSTR wifi_name, LPCTSTR wifi_pwd);

	bool GetAllLocalIP(std::vector<std::wstring> &ips);

private:
	bool DoShellExe(LPCTSTR comline);

private:
	HANDLE hClient_;
};