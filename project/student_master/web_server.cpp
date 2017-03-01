#include "web_server.h"
#include "main_wnd.h"
#include "application.h"


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
	try {
		WebServer* pThis = static_cast<WebServer*>(arg);
		request_data tmp;
		char dest[100] = "\0";
		tmp.first = req;
		tmp.second = true;

		char* post_data = ((char*)EVBUFFER_DATA(req->input_buffer));
		memcpy(dest, (char*)EVBUFFER_DATA(req->input_buffer), EVBUFFER_LENGTH(req->input_buffer));
		post_data[EVBUFFER_LENGTH(req->input_buffer)] = '\0';

		JsonOperate json_operator;
		StudentData student_data;
		json_operator.JsonAnalysis(post_data, student_data);

		if (student_data.operate_type_ == OperateType::KEEPA_LIVE)
		{
			evbuffer* buf = evbuffer_new();
			evbuffer_add_printf(buf, pThis->broadcast_ip_.c_str(), evhttp_request_get_uri(req));
			evhttp_send_reply(req, HTTP_OK, "OK", buf);
		}
		else {
			pThis->req_vec_.push_back(tmp);
		}
	}
	catch (std::exception& e) {
		OutputDebugStringA(e.what());
	}
}

void WebServer::TimeOutCallback(evutil_socket_t fd, short event, void * arg)
{
	try {
		char output[2048] = "\0";
		char tmp[1024] = "\0";
		char dest[100] = "\0";
		WebServer* pThis = static_cast<WebServer*>(arg);
#ifdef _DEBUG
		OutputDebugStringA("time out callback \n");
#endif // _DEBUG

		if (pThis->req_vec_.empty())
			return;
		if (pThis->req_vec_.front().second) {
			evbuffer* buf = evbuffer_new();
			if (!buf) {
				return;
			}
			char* post_data = ((char*)EVBUFFER_DATA(pThis->req_vec_.front().first->input_buffer));
			memcpy(dest, (char*)EVBUFFER_DATA(pThis->req_vec_.front().first->input_buffer), EVBUFFER_LENGTH(pThis->req_vec_.front().first->input_buffer));
			post_data[EVBUFFER_LENGTH(pThis->req_vec_.front().first->input_buffer)] = '\0';

			JsonOperate json_operate;
			StudentData student_data;
			json_operate.JsonAnalysis(post_data, student_data);

			switch (student_data.operate_type_) {
			case OperateType::LOGON: {
				LogonInfo ret_logon;
				ret_logon = *pThis->mssqlo_->Query(ATL::CA2W(student_data.appid_.c_str()));
				std::string json_str = json_operate.AssembleJson(ret_logon);

				evbuffer_add_printf(buf, json_str.c_str(), evhttp_request_get_uri(pThis->req_vec_.front().first));
#ifdef _DEBUG
				OutputDebugStringA("logon respons succeed \n");
#endif // _DEBUG
			}
									 break;

			case OperateType::HANDUP: {
				StudentData tmp_handup;
				//发送消息给主界面弹出提示
				PostMessage((HWND)App::GetInstance()->GetMainWnd(), Silent_Handup, (WPARAM)(tmp_handup.stream_ip_.c_str()), 0);

				evbuffer_add_printf(buf, "null", evhttp_request_get_uri(pThis->req_vec_.front().first));
			}
									  break;
			default:
				evbuffer_add_printf(buf, "undefined command", evhttp_request_get_uri(pThis->req_vec_.front().first));
				break;
			}
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

	mssqlo_.reset(new MsSqlDbOperate);
	mssqlo_->Connect(L"SQS", L"sa", L"123");

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
#ifdef DEBUG
		OutputDebugStringA("server initial faild");
#endif // DEBUG

		return -1;
	}

	event_assign(&timeout_, base_, -1, EV_PERSIST, TimeOutCallback, this);
	evutil_timerclear(&timevalue_);
	timevalue_.tv_sec = time_out;
	event_add(&timeout_, &timevalue_);

	evhttp_set_gencb(http_server_, &WebServer::HttpResponse, this);
	return 0;
}

void WebServer::ServerStart()
{
	event_base_dispatch(base_);
}

void WebServer::SetStreamIp(char * stream_ip)
{
	assert(stream_ip);
	broadcast_ip_ = stream_ip;
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