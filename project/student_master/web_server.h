#ifndef __WEB_SERVER__
#define __WEB_SERVER__

#include <evhttp.h>
#include <http.h>
#include <string>
#include <vector>
#include <thread>
#include <assert.h>
#include <list>
#include <memory>
#include <io.h>
#include "sql_server_operate.h"


typedef void(*pFunc)(struct evhttp_request *, void *);
typedef std::pair<evhttp_request*, bool> request_data;
class WebServer
{
public:
	WebServer();
	~WebServer();

	static void HttpResponse(evhttp_request* req, void* arg);
	static void TimeOutCallback(evutil_socket_t fd, short event, void *arg);

	//char* Logon();

	void HttpDisposal(evhttp_request* req, void* arg);
	
	int Initial(int time_out, char* http_addr, short http_port);

	void ServerStart();

private:
	std::shared_ptr<MsSqlDbOperate> mssqlo_;
	std::list<request_data> req_vec_;
	event_base* base_;
	evhttp* http_server_;
	event timeout_;
	timeval timevalue_;
	pFunc this_func_;
	bool handup_;
};
#endif //__WEB_SERVER__