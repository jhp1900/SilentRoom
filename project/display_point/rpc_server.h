#pragma once

#include "student_hand_h.h"
#include <string>
#include <assert.h>

#pragma comment(lib, "rpcrt4")
#pragma comment(lib, "ole32")

class RpcServer
{
public:
	RpcServer();
	~RpcServer();

	bool Initial(std::string port = "12322");
	bool RpcListen();

	bool SetRpcServerPort(const char* port);

private:
	std::string listen_port_;
};
