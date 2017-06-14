#include "rpc_client.h"
#include <assert.h>
#include "..\RpcIDL\student_hand_h.h"

#pragma comment(lib, "rpcrt4")
#pragma comment(lib, "ole32")

void __RPC_FAR* __RPC_USER midl_user_allocate(size_t len)
{
	return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR *ptr)
{
	free(ptr);
}

RpcClient::RpcClient()
	:pszStringBinding_(NULL)
	,test_ip_("")
	,test_port_("")
{
}


RpcClient::~RpcClient()
{
	RpcStringFreeA(&pszStringBinding_);
	RpcBindingFree(&Handup_Binding);
}

void RpcClient::BindRpcServer(const char *ip, const char *port)
{
	SetDisplayPointIp(ip);
	SetDisplayPointPort(port);

	RpcStringBindingComposeA(NULL
		,(unsigned char*)"ncacn_ip_tcp"
		,(unsigned char*)test_ip_.c_str()
		,(unsigned char*)test_port_.c_str()
		,NULL
		,&pszStringBinding_);

	RpcBindingFromStringBindingA(pszStringBinding_, &Handup_Binding);
}

void RpcClient::SetDisplayPointIp(const char * ip)
{
	assert(ip);
	test_ip_ = ip;
}

void RpcClient::SetDisplayPointPort(const char * port)
{
	assert(port);
	test_port_ = port;
}

bool RpcClient::HandupOperat(const char* student_data)
{
	bool ret = true;
	RpcTryExcept{
		Handup((unsigned char*)student_data);
	} RpcExcept(1) {
		printf("RPC Exception %d\n", RpcExceptionCode());
		ret = false;
	}
	RpcEndExcept

	return ret;
}

