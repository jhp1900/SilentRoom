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

	bool HandupOperat();
	bool HandupEnd();

private:
	std::string test_ip_, test_port_;
	unsigned char* pszStringBinding_;
};

void __RPC_FAR* __RPC_USER midl_user_allocate(size_t len)
{
	return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR *ptr)
{
	free(ptr);
}