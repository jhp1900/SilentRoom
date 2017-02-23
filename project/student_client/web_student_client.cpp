#include "web_student_client.h"
#include "application.h"
#include "main_wnd.h"
#include "msg_head.h"

//获取WEB服务器返回数据方法
static size_t GetData(void *ptr, size_t size, size_t nmemb, void *userdata) {
	std::string* server_data = static_cast<std::string*>(userdata);
	server_data->append((char*)ptr, size* nmemb);
	HWND hwnd = App::GetInstance()->GetMainWnd()->GetHWND();
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
			curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 2);			// 设置10S超时返回
		}
	}
	catch (const std::exception& exc) {
		printf("%s\n", exc.what());
	}
}

void WebStudentClient::SendWebMessage(std::string msg)
{
	try {
		curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, msg.c_str());

		res_ = curl_easy_perform(curl_);
		if (res_ != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() faild: %s\n", curl_easy_strerror(res_));
		}
		//curl_easy_cleanup(curl_);
		//curl_global_cleanup();
	}
	catch (const std::exception& exc) {
		printf("%s\n", exc.what());
	}
}
