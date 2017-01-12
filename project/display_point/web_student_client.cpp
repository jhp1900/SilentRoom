#include "web_student_client.h"



WebStudentClient::WebStudentClient()
{
}


WebStudentClient::~WebStudentClient()
{
	curl_global_cleanup();
}

void WebStudentClient::Initial()
{
	try {
		curl_global_init(CURL_GLOBAL_ALL);

		curl_ = curl_easy_init();

		if (curl_) {
			curl_easy_setopt(curl_, CURLOPT_URL, "http://10.18.3.67:8081");
		}
	}
	catch (const std::exception& exc) {
		printf("%s\n", exc.what());
	}
}

void WebStudentClient::SendWebMessage()
{
	try {
		curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, "i need XXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

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
