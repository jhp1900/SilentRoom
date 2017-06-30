#include "web_student_client.h"
#include "msg_head.h"
#include <thread>
#include <mutex>


#include "..\student_client\application.h"
#include "..\student_client\main_wnd.h"

//#include "..\display_point\application.h"
//#include "..\display_point\main_wnd.h"

std::mutex g_lock;

//获取WEB服务器返回数据方法
static size_t GetData(void *ptr, size_t size, size_t nmemb, void *userdata) {
	char* server_data = (char*)(userdata);
	memcpy(server_data, (char*)ptr, size* nmemb);
	HWND hwnd = App::GetInstance()->GetMainWnd()->GetHWND();
	OutputDebugStringA(server_data);
	PostMessage(hwnd, kAM_WebRetMsg, WPARAM(server_data), 0);
	return (size* nmemb);
}

WebStudentClient::WebStudentClient()
{
}

WebStudentClient::~WebStudentClient()
{
	curl_global_cleanup();
}

bool WebStudentClient::Initial(std::string url)
{
	try {
		curl_global_init(CURL_GLOBAL_ALL);

		curl_ = curl_easy_init();

		if (curl_) {
			curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());		// url = "http://10.18.3.67:8081"
			curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, GetData);
			curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &buf_);
			curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 2);			// 设置2S超时返回
		}
	}
	catch (const std::exception& exc) {
		printf("%s\n", exc.what());
		return false;
	}
	return true;
}

void WebStudentClient::SendWebMessage(std::string msg)
{
	auto SendMsgThread = [&](std::string msg) {
		try {
			g_lock.lock();
			curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, msg.c_str());
			CURLcode res = curl_easy_perform(curl_);
			g_lock.unlock();

			if (res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() faild: %s\n", curl_easy_strerror(res));
			}
		} catch (const std::exception& exc) {
			printf("%s\n", exc.what());
		}
	};

	std::thread send_thread(SendMsgThread, msg);
	send_thread.detach();
}

void WebStudentClient::SendWebMessage(std::string msg, bool sleep_time)
{
	auto SendMsgThread = [&](std::string msg, bool sleep) {
		try {
			StudentData keep_alive;
			JsonOperate json_operate;
			do
			{
				if (sleep) {
					keep_alive.appid_ = "1";
					keep_alive.handup_ = false;
					keep_alive.naem_ = "xx";
					keep_alive.operate_type_ = KEEPA_LIVE;
					keep_alive.sno_ = "1";
					keep_alive.stream_ip_ = "null";
					OutputDebugStringA("keep alive \n");
					std::this_thread::sleep_for(std::chrono::milliseconds(5000));
				}

				g_lock.lock();
				if (sleep)
					curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, json_operate.AssembleJson(keep_alive));
				else
				curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, msg.c_str());
				CURLcode res = curl_easy_perform(curl_);
				g_lock.unlock();

				if (res != CURLE_OK) {
					fprintf(stderr, "curl_easy_perform() faild: %s\n", curl_easy_strerror(res));
					OutputDebugStringA("server not response! \n");
				}
			} while (sleep);
			OutputDebugStringA("keep alive thread is end \n");
			printf("%d", sleep);
		}
		catch (const std::exception& exc) {
			printf("%s\n", exc.what());
		}
	};

	std::thread send_thread(SendMsgThread, msg, sleep_time);
	send_thread.detach();
}
