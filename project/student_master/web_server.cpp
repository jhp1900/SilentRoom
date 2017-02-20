#include "web_server.h"
#include "main_wnd.h"
#include "application.h"


WebServer::WebServer()
{
	mssqlo_->Connect(L"SQS", L"sa", L"123");
}


WebServer::~WebServer()
{
	evhttp_free(http_server_);
	WSACleanup();
}

void WebServer::HttpResponse(evhttp_request * req, void * arg)
{
	WebServer* pThis = static_cast<WebServer*>(arg);
	request_data tmp;
	tmp.first = req;
	tmp.second = false;

	pThis->req_vec_.push_back(tmp);
}

void WebServer::TimeOutCallback(evutil_socket_t fd, short event, void * arg)
{
	try {
		WebServer* pThis = static_cast<WebServer*>(arg);
		if (pThis->req_vec_.empty())
			return;
		if (pThis->req_vec_.front().second) {
			evbuffer* buf = evbuffer_new();
			if (!buf) {
				return;
			}

			char* post_data = ((char*)EVBUFFER_DATA(pThis->req_vec_.front().first->input_buffer));
			JsonOperate json_operate;
			StudentData student_data = json_operate.JsonAnalysis(post_data);

			switch (student_data.operateType_){
			case logon:{
				StudentData tmp;
				tmp = *pThis->mssqlo_->Query(ATL::CA2W(student_data.student_name_.c_str()));
				evbuffer_add_printf(buf, json_operate.AssembleJson(tmp), evhttp_request_get_uri(pThis->req_vec_.front().first));
			}
				break;
			case handup:{
				StudentData tmp_handup;
				tmp_handup = *pThis->mssqlo_->Query(ATL::CA2W(student_data.student_name_.c_str()));
				//发送消息给主界面弹出提示
				bool* boolptr = nullptr;
				*boolptr = false;
				//SendMessage((HWND)App::GetInstance()->GetMainWnd(), );
				if (*boolptr) {
					evbuffer_add_printf(buf, tmp_handup.group_info_.c_str(), evhttp_request_get_uri(pThis->req_vec_.front().first));
				}

				evbuffer_add_printf(buf, "null", evhttp_request_get_uri(pThis->req_vec_.front().first));
			}
				break;
			case keepalive: {

			}
				break;
			default:
				break;
				evbuffer_add_printf(buf, "undefined command", evhttp_request_get_uri(pThis->req_vec_.front().first));
			}

			//evbuffer_add_printf(buf, "Server Responsed OK \n", evhttp_request_get_uri(pThis->req_vec_.front().first)); //响应请求

			evhttp_send_reply(pThis->req_vec_.front().first, HTTP_OK, NULL, buf);
			evbuffer_free(buf);
			pThis->req_vec_.pop_front();
		}
	}
	catch (std::exception& e) {
		OutputDebugStringA(e.what());
	}
}

int WebServer::Initial(int time_out, char* http_addr, short http_port)
{
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

	event_assign(&timeout_, base_, -1, EV_PERSIST, TimeOutCallback, this);
	evutil_timerclear(&timevalue_);
	timevalue_.tv_sec = 2;
	event_add(&timeout_, &timevalue_);

	evhttp_set_gencb(http_server_, &WebServer::HttpResponse, this);
	return 0;
}

void WebServer::ServerStart()
{
	event_base_dispatch(base_);
}

void WebServer::HttpDisposal(evhttp_request * req, void * arg)
{
	evbuffer* buf = evbuffer_new();
	assert(buf);
	//return -1;

	char output[] = "0";
	char tmp[] = "0";
	char dest[] = "0";
	evbuffer_add_printf(buf, "Responed Ok", evhttp_request_get_uri(req));

	memcpy(dest, (char*)EVBUFFER_DATA(req->input_buffer), EVBUFFER_LENGTH(req->input_buffer));
	char* post_data = (char*)(EVBUFFER_DATA(req->input_buffer));
	post_data[EVBUFFER_LENGTH(req->input_buffer)] = '\0';


	evhttp_send_reply(req, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
	//return 0;
}