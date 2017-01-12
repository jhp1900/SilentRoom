#pragma once

#include <string>
#include <assert.h>
#include "student_hand_h.h"

#pragma comment(lib, "rpcrt4")
#pragma comment(lib, "ole32")

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
	RPC_ASYNC_STATE async_;
	std::string test_ip_, test_port_;
	unsigned char* pszStringBinding_;
};
