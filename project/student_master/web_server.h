#ifndef __WEB_SERVER__
#define __WEB_SERVER__

#include <evhttp.h>
#include <http.h>
#include <string>
#include <vector>
#include <thread>
#include <assert.h>

class WebServer
{
public:
	WebServer();
	~WebServer();

	static void HttpResponse(evhttp_request* req, void* arg);
	void HttpDisposal(evhttp_request* req, void* arg);

	int Initial(int time_out, char* http_addr, short http_port);

	void ServerStart();

private:
	event_base* base_;
	evhttp* http_server_;

};
#endif //__WEB_SERVER__