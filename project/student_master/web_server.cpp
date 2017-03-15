#include "web_server.h"
#include "main_wnd.h"
#include "application.h"
#include <map>

WebServer::WebServer()
{
	handup_return_data_.reset(new StudentData);
	handup_return_data_->appid_ = "1";
	handup_return_data_->handup_ = false;
	handup_return_data_->naem_ = "null";
	handup_return_data_->operate_type_ = KEEPA_LIVE;
	handup_return_data_->sno_ = "1";
	handup_return_data_->stream_ip_ = "null";
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
		tmp.first = req;
		tmp.second = true;

		switch (req->type)
		{
		case EVHTTP_REQ_POST: {
			char* post_data = ((char*)EVBUFFER_DATA(req->input_buffer));
			post_data[EVBUFFER_LENGTH(req->input_buffer)] = '\0';

			JsonOperate json_operator;
			StudentData student_data;
			json_operator.JsonAnalysis(post_data, student_data);
			if (student_data.operate_type_ == OperateType::KEEPA_LIVE)
			{
				evbuffer* buf = evbuffer_new();
				StudentData data = *(pThis->handup_return_data_);
				char debug_text[MAX_PATH];
				sprintf(debug_text, json_operator.AssembleJson(data));
				OutputDebugStringA(debug_text);
				evbuffer_add_printf(buf, json_operator.AssembleJson(data), evhttp_request_get_uri(req));
				evhttp_send_reply(req, HTTP_OK, "OK", buf);
			} else {
				pThis->server_metux_.lock();
				pThis->req_vec_.push_back(tmp);
				pThis->server_metux_.unlock();
				OutputDebugStringA("hand up \n");
			}
		}
			break;
		case EVHTTP_REQ_GET: {
			/*evbuffer* buf = evbuffer_new();
			char* post_data = ((char*)EVBUFFER_DATA(req->input_buffer));
			post_data[EVBUFFER_LENGTH(req->input_buffer)] = '\0';

			evbuffer* buf = evbuffer_new();
			evkeyvalq valkey;
			const char* req_str = evhttp_request_get_uri(req);
			evhttp_parse_query(req_str, &valkey);
			evkeyval* val = valkey.tqh_first;
			char* key = val->key;
			char* value = val->value;

			JsonOperate json_operator;
			string xx = json_operator.AssembleJson(pThis->master_data_);
			evbuffer_add_printf(buf, json_operator.AssembleJson(pThis->master_data_), evhttp_request_get_uri(req));
			evhttp_send_reply(req, HTTP_OK, "OK", 0);*/

			/* 以下为测试代码 */
			evbuffer* buf = evbuffer_new();
			JsonOperate json_operator;
			evkeyvalq keyvalq;
			const char *req_str = evhttp_request_get_uri(req);
			evhttp_parse_query(req_str, &keyvalq);
			evkeyval *val = keyvalq.tqh_first;
			std::map<string, string> dt_map;
			while (val) {
				dt_map[val->key] = val->value;
				val = val->next.tqe_next;
			}

			const char * json_str = "";

			/* 加载学生状态表 */
			auto reload_stu = [&]() {
				json_str = json_operator.AssembleJson(pThis->mssqlo_->master_data_);
			};

			/* 加载分组信息表 */
			auto reload_gp_mng = [&]() {
				json_str = json_operator.AssembleJson(pThis->mssqlo_->group_manager_);
			};

			/* 加载小组IP信息 */
			auto reload_gp_ip = [&]() {
				json_str = json_operator.AssembleJson(pThis->mssqlo_->group_ip_);
			};

			/*删除学生机*/
			auto delete_stu = [=](std::string appid) {
				pThis->mssqlo_->DeleteAppid(ATL::CA2W(appid.c_str()));
			};

			/*添加小组IP*/
			auto add_group_ip = [=](std::string group, std::string value) {
				pThis->mssqlo_->AddGroupIp(ATL::CA2W(group.c_str()), ATL::CA2W(value.c_str()));
			};

			/*更新小组信息*/
			auto update_group_ip = [=](std::string group, std::string value) {
				pThis->mssqlo_->UpdateGroupIp(ATL::CA2W(value.c_str()), ATL::CA2W(group.c_str()));
			};

			/*更新APPID*/
			auto update_appid = [=](std::string appid, std::string group) {
				pThis->mssqlo_->UpdateAppid(ATL::CA2W(appid.c_str()), ATL::CA2W(group.c_str()));
			};

			/*添加APPID*/
			auto add_appid = [=](std::string appid, std::string group) {
				pThis->mssqlo_->AddAppid(ATL::CA2W(appid.c_str()), ATL::CA2W(group.c_str()));
			};

			auto delete_group = [=](std::string group) {
				pThis->mssqlo_->DeleteGroup(ATL::CA2W(group.c_str()));
			};

			auto handle_student_speak = [=](std::string sno) {
				App::GetInstance()->GetWebServer()->broadcast_ip_ = ATL::CW2A(pThis->mssqlo_->QueryStudentIp(ATL::CA2W(sno.c_str())).c_str());
				OutputDebugStringA(App::GetInstance()->GetWebServer()->broadcast_ip_.c_str());
			};

			if (dt_map["control"] == "reload") {
				if (dt_map["range"] == "students") {				// 加载学生状态表
					reload_stu();
				} else if(dt_map["range"] == "group_mng") {			// 加载分组信息表
					reload_gp_mng();
				} else if (dt_map["range"] == "group_ip") {			// 加载小组IP信息
					reload_gp_ip();
				}
			} else if (dt_map["control"] == "speak") {
				if (dt_map["range"] == "to_group") {				// 小组内发言
					//...
				} else if (dt_map["range"] == "to_class") {			// 班级发言
					std::thread handle_student_speak_thread(handle_student_speak, dt_map["obj"]);
					handle_student_speak_thread.detach();	
				}
			} else if (dt_map["control"] == "updata") {
				if (dt_map["range"] == "group_mng") {				// 修改分组信息
					std::thread update_appid_thread(update_appid, dt_map["obj"], dt_map["value"]);
					update_appid_thread.detach();
				} else if (dt_map["range"] == "group_ip") {			// 修改小组IP
					std::thread update_group_thread(update_group_ip, dt_map["values"], dt_map["obj"]);
					update_group_thread.detach();
				}
			} else if (dt_map["control"] == "add") {
				if (dt_map["range"] == "group_mng") {				// 添加 学生机信息
					std::thread add_appid_thread(add_appid, dt_map["obj"], dt_map["values"]);
					add_appid_thread.detach();
				} else if (dt_map["range"] == "group_ip") {			// 添加小组IP
					std::thread add_group_thread(add_group_ip, dt_map["obj"], dt_map["values"]);
					add_group_thread.detach();
				}
			} else if (dt_map["control"] == "delete") {
				if (dt_map["range"] == "group_mng") {				// 删除某个学生机
					std::thread stu_thread(delete_stu, dt_map["obj"]);
					stu_thread.detach();
				} else if (dt_map["range"] == "group_ip") {			// 删除某个小组（IP以及改组的所有信息）
					std::thread delete_group_thread(delete_group, dt_map["obj"]);
					delete_group_thread.detach();
				}
			}

			evbuffer_add_printf(buf, json_str, req_str);
			evhttp_send_reply(req, HTTP_OK, "OK", buf);
		}
			break;
		default:
			break;
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
		//char* dest;
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
			//dest = (new char[EVBUFFER_LENGTH(pThis->req_vec_.front().first->input_buffer)]);
			//memcpy(dest, (char*)EVBUFFER_DATA(pThis->req_vec_.front().first->input_buffer), EVBUFFER_LENGTH(pThis->req_vec_.front().first->input_buffer));
			post_data[EVBUFFER_LENGTH(pThis->req_vec_.front().first->input_buffer)] = '\0';

			JsonOperate json_operate;
			StudentData student_data;
			json_operate.JsonAnalysis(post_data, student_data);

			/* 登录消息处理 */
			auto on_logon = [&]() {
				LogonInfo ret_logon;
				ret_logon = *pThis->mssqlo_->Query(ATL::CA2W(student_data.appid_.c_str()));
				std::string json_str = json_operate.AssembleJson(ret_logon);
				evbuffer_add_printf(buf, json_str.c_str(), evhttp_request_get_uri(pThis->req_vec_.front().first));
			};

			/* 举手消息处理 */
			auto on_handup = [&](std::string streamip) {
				pThis->handup_return_data_->appid_ = student_data.appid_;
				pThis->handup_return_data_->handup_ = false;
				pThis->handup_return_data_->naem_ = student_data.naem_;
				//pThis->handup_return_data_->operate_type_ = student_data.operate_type_;
				pThis->handup_return_data_->sno_ = student_data.sno_;
				pThis->broadcast_ip_ = student_data.stream_ip_;
				//StudentData tmp_handup;
				//发送消息给主界面弹出提示
				PostMessage(App::GetInstance()->GetMainWnd()->GetHWND(), kAM_Silent_Handup, (WPARAM)(streamip.c_str()), 0);
			};

			/* 学生小组内发言或停止发言 */
			auto on_speak_or_stop = [&]() {
				int ret = pThis->mssqlo_->Update(ATL::CA2W(student_data.sno_.c_str()), student_data.handup_);
				// TODO ......
				// 更新数据库状态（注：无需 evbuffer_add_printf 返回数据）
			};

			switch (student_data.operate_type_) {
				case OperateType::LOGON: on_logon(); break;
				case OperateType::HANDUP: on_handup(student_data.stream_ip_); break;
				case OperateType::SPEAK:					// 处理函数同下
				case OperateType::STOP_SPEAK: on_speak_or_stop();  break;
				default:
					break;
			}
			evhttp_send_reply(pThis->req_vec_.front().first, HTTP_OK, NULL, buf);
			evbuffer_free(buf);
			pThis->server_metux_.lock();
			pThis->req_vec_.pop_front();
			pThis->server_metux_.unlock();
		}
	}
	catch (std::exception& e) {
		OutputDebugStringA(e.what());
	}
}

int WebServer::Initial(int time_out, const char* http_addr, short http_port)
{
	WSADATA ws_data;

	mssqlo_.reset(new MsSqlDbOperate);
	mssqlo_->Connect(L"SQS", L"sa", L"123");
	auto query_thread = [&]() {
		while (true)
		{
			mssqlo_->QueryStatus();
			mssqlo_->QueryGroupManager();
			mssqlo_->QueryGroupIP();
			this_thread::sleep_for(chrono::milliseconds(2000));
#ifdef DEBUG
			OutputDebugStringA("query server status \n");
#endif
		}
	};
	std::thread sql_thread(query_thread);
	sql_thread.detach();
	mssqlo_->DeleteGroup(L"Group1");
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
#ifdef DEBUG
	char outputserver[MAX_PATH];
	sprintf(outputserver,"server:%s:%d", http_addr, http_port);
	OutputDebugStringA(outputserver);
#endif // DEBUG
	event_assign(&timeout_, base_, -1, EV_PERSIST, TimeOutCallback, this);
	evutil_timerclear(&timevalue_);
	timevalue_.tv_sec = time_out;
	event_add(&timeout_, &timevalue_);

	evhttp_set_gencb(http_server_, &WebServer::HttpResponse, this);
	return 0;
}

void WebServer::ServerStart()
{
	auto start_server = [&]() {
		event_base_dispatch(base_);
	};
	std::thread server_thread(start_server);
	server_thread.detach();
}

std::shared_ptr<StudentData> WebServer::GetHandupData()
{
	return handup_return_data_;
}

void WebServer::SetStreamIp(const char* stream_ip)
{
	handup_return_data_->handup_ = true;
	handup_return_data_->stream_ip_ = broadcast_ip_;
}

std::string WebServer::GetStreamIp()
{
	return broadcast_ip_;
}

std::shared_ptr<StudentData> WebServer::GetStudentReturnData()
{
	return handup_return_data_;
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