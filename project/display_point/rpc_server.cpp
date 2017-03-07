#include "rpc_server.h"
#include "application.h"
#include "main_wnd.h"
#include <assert.h>
#include "..\RpcIDL\student_hand_h.h"
#pragma comment(lib, "rpcrt4")
#pragma comment(lib, "ole32")

void Handup(unsigned char* start) {
	
	::PostMessage(App::GetInstance()->GetMainWnd()->GetHWND(), kAM_RpcHandupMsg, (WPARAM)start, 0);
	//RpcAsyncCompleteCall(rpcasynhandle, "ok");
}

void HandupEnd(unsigned char* end) {
	printf("recieve");
	printf("%s\n", end);
	//RpcAsyncCompleteCall(rpcasynhandle, NULL);
}

void* __RPC_USER midl_user_allocate(size_t len) {
	return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR* ptr) {
	free(ptr);
}

RpcServer::RpcServer()
{
}


RpcServer::~RpcServer()
{
}

bool RpcServer::Initial(std::string port /*= "12322"*/)
{
	RpcServerUseProtseqEpA((unsigned char*)"ncacn_ip_tcp", RPC_C_PROTSEQ_MAX_REQS_DEFAULT, (unsigned char*)port.c_str(), NULL);
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
	return true;
}

bool RpcServer::SetRpcServerPort(const char * port)
{
	assert(port);
	listen_port_ = port;
	return true;
}
