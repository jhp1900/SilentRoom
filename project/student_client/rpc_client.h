#pragma once

#include <string>



class RpcClient
{
public:
	RpcClient();
	~RpcClient();

	void BindRpcServer(const char *ip, const char *port);

	//≤‚ ‘”√
	void SetDisplayPointIp(const char* ip);
	void SetDisplayPointPort(const char* port);

	bool HandupOperat(const char* student_data);

private:
	std::string test_ip_, test_port_;
	unsigned char* pszStringBinding_;
};
