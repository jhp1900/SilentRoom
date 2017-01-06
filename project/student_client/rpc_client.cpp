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

void RpcClient::Initial()
{
	//unsigned char* pszStringBinding_ = NULL;
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

bool RpcClient::HandupOperat()
{
	RPC_ASYNC_STATE async;
	RpcAsyncInitializeHandle(&async, sizeof(async));
	async.UserInfo = NULL;
	async.NotificationType = RpcNotificationTypeNone;

	//×é×°JSON
	std::string student_data = "test_handup";

	Handup(&async, (unsigned char*)student_data.c_str());

	RpcAsyncCompleteCall(&async, NULL);
	return false;
}

bool RpcClient::HandupEnd()
{
	RpcStringFreeA(&pszStringBinding_);
	RpcBindingFree(&Handup_Binding);

	return true;
}
