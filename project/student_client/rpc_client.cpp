#include "rpc_client.h"



RpcClient::RpcClient()
	:pszStringBinding_(NULL)
	,test_ip_("10.18.3.67")
	,test_port_("2016")
{
}


RpcClient::~RpcClient()
{
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
	RPC_ASYNC_STATE async;
	RpcTryExcept{

	RpcAsyncInitializeHandle(&async, sizeof(async));
	async.UserInfo = NULL;
	async.NotificationType = RpcNotificationTypeNone;

	Handup(&async, (unsigned char*)student_data);

	while (RpcAsyncGetCallStatus(&async) == RPC_S_ASYNC_CALL_PENDING)
	{
		//printf("call hello() pending, wait 1s...\n");
		Sleep(50);
	}

	RpcAsyncCompleteCall(&async, NULL);
	}
		RpcExcept(1) {
		printf("RPC Exception %d\n", RpcExceptionCode());
	}

	RpcEndExcept

	RpcStringFreeA(&pszStringBinding_);
	RpcBindingFree(&Handup_Binding);

	return true;
}

