#include "web_student_client.h"
#include "msg_head.h"
#include <thread>
#include <mutex>


//#include "..\student_client\application.h"
//#include "..\student_client\main_wnd.h"

#include "..\display_point\application.h"
#include "..\display_point\main_wnd.h"

std::mutex g_lock;

//获取WEB服务器返回数据方法
static size_t GetData(void *ptr, size_t size, size_t nmemb, void *userdata) {
	std::string* server_data = static_cast<std::string*>(userdata);
	server_data->append((char*)ptr, size* nmemb);
	HWND hwnd = App::GetInstance()->GetMainWnd()->GetHWND();
	if (server_data->find("rtmp://") && server_data->find("hanup")) //心跳返回数据
		PostMessage(hwnd, NULL, WPARAM(server_data), 0);
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

void WebStudentClient::Initial(std::string url)
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
	}
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

void WebStudentClient::KeepAlive(std::string msg)
{
	auto SendMsgThread = [&](std::string msg) {
		try {
			while (true)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				g_lock.lock();
				curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, msg.c_str());
				CURLcode res = curl_easy_perform(curl_);
				g_lock.unlock();

				if (res != CURLE_OK) {
					fprintf(stderr, "curl_easy_perform() faild: %s\n", curl_easy_strerror(res));
					OutputDebugStringA("server not response! \n");
				}
			}
		}
		catch (const std::exception& exc) {
			printf("%s\n", exc.what());
		}
	};

	std::thread send_thread(SendMsgThread, msg);
	send_thread.detach();
}
