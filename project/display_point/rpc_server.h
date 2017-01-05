#pragma once

#include "student_hand_h.h"
#include <string>

#pragma comment(lib, "rpcrt4")
#pragma comment(lib, "ole32")

class RpcServer
{
public:
	RpcServer();
	~RpcServer();

	bool Initial();
	bool RpcListen();

private:
	std::string listen_port_;
};

void Handup(PRPC_ASYNC_STATE rpcasynhandle, unsigned char* start) {

	RpcAsyncCompleteCall(rpcasynhandle, NULL);
}

void HandupEnd(PRPC_ASYNC_STATE rpcasynhandle, unsigned char* start) {

	RpcAsyncCompleteCall(rpcasynhandle, NULL);
}

void* __RPC_USER midl_user_allocate(size_t len) {
	return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR* ptr) {
	free(ptr);
}