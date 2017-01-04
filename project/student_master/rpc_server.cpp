#include "rpc_server.h"



RpcServer::RpcServer()
{
}


RpcServer::~RpcServer()
{
}

bool RpcServer::Initial()
{
	RpcServerUseProtseqEp((unsigned short*)"ncacn_ip_tcp", RPC_C_PROTSEQ_MAX_REQS_DEFAULT, (unsigned short*)listen_port_.c_str(), NULL);
	RpcServerRegisterIfEx(StudentHand_v1_0_s_ifspec
		,NULL
		,NULL
		,RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH
		,0
		,NULL);
	return true;
}

bool RpcServer::RpcListen()
{
	RpcServerListen(1, RPC_C_PROTSEQ_MAX_REQS_DEFAULT, FALSE);
	return false;
}
