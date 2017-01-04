#include "rpc_client.h"



RpcClient::RpcClient()
	:pszStringBinding_(NULL)
{
}


RpcClient::~RpcClient()
{
}

void RpcClient::Initial()
{
	RpcStringBindingCompose(NULL
		,(unsigned short*)"ncacn_ip_tcp"
		,(unsigned short*)test_ip_.c_str()
		,(unsigned short*)test_port_.c_str()
		,NULL
		,&pszStringBinding_);

	RpcBindingFromStringBinding(pszStringBinding_, &Handup_Binding);
}

bool RpcClient::HandupOperat()
{
	RPC_ASYNC_STATE async;
	RpcAsyncInitializeHandle(&async, sizeof(async));
	async.UserInfo = NULL;
	async.NotificationType = RpcNotificationTypeNone;

	//��װJSON
	std::string student_data;

	Handup(&async, (unsigned char*)student_data.c_str());

	RpcAsyncCompleteCall(&async, NULL);
	return false;
}

bool RpcClient::HandupEnd()
{
	RpcStringFree(&pszStringBinding_);
	RpcBindingFree(&Handup_Binding);

	return true;
}
