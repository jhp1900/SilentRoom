#include "web_server.h"



WebServer::WebServer()
{
}


WebServer::~WebServer()
{
	evhttp_free(http_server_);
	WSACleanup();
}

void WebServer::HttpResponse(evhttp_request * req, void * arg)
{
	WebServer* pThis = static_cast<WebServer*>(arg);
	std::vector<evhttp_request> req_vec;
	req_vec.push_back(*req);

	auto chirld_thread = new std::thread(std::bind(&WebServer::HttpDisposal, pThis, req, arg));
}

int WebServer::HttpDisposal(evhttp_request * req, void * arg)
{
	evbuffer* buf = evbuffer_new();
	assert(buf);
	return -1;

	char output[] = "0";
	char tmp[] = "0";
	char dest[] = "0";
	evbuffer_add_printf(buf, "Responed Ok", evhttp_request_get_uri(req));

	memcpy(dest, (char*)EVBUFFER_DATA(req->input_buffer), EVBUFFER_LENGTH(req->input_buffer));
	char* post_data = (char*)(EVBUFFER_DATA(req->input_buffer));
	post_data[EVBUFFER_LENGTH(req->input_buffer)] = '\0';


	evhttp_send_reply(req, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
	return 0;
}

int WebServer::Initial(int time_out, char* http_addr, short http_port)
{
	//int http_option_timeout = time_out;
	//short http_port = http_port;

	WSADATA ws_data;
	if (WSAStartup(MAKEWORD(2, 2), &ws_data) != 0) {
		return -1;
	}
	
	base_ = event_base_new();
	http_server_ = evhttp_new(base_);

	if (!http_server_) {
		return -1;
	}

	int ret = evhttp_bind_socket(http_server_, http_addr, http_port);
	if (ret != 0) {
		return -1;
	}
	//this_func_ = reinterpret_cast<pFunc>(std::bind(&WebServer::HttpResponse, this, std::placeholders::_1, std::placeholders::_2));
	//this_func_ = (pFunc)&WebServer::HttpResponse;
	evhttp_set_gencb(http_server_, &WebServer::HttpResponse, this);
	return 0;
}

void WebServer::ServerStart()
{
	event_base_dispatch(base_);
}