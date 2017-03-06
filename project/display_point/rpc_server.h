#pragma once

#include <string>

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
