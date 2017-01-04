#pragma once

#include <string>
#include "student_hand_h.h"

#pragma comment(lib, "rpcrt4")
#pragma comment(lib, "ole32")

class RpcClient
{
public:
	RpcClient();
	~RpcClient();

	void Initial();

	bool HandupOperat();
	bool HandupEnd();

private:
	std::string test_ip_, test_port_;
	unsigned short* pszStringBinding_;
};

void __RPC_FAR* __RPC_USER midl_user_allocate(size_t len)
{
	return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR *ptr)
{
	free(ptr);
}