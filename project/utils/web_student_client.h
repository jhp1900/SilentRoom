#ifndef __WEB_STUDENT_CLIENT__
#define __WEB_STUDENT_CLIENT__

#include <curl\curl.h>
#include <exception>
#include <string>
#include <thread>
#include "json_operate.h"

class WebStudentClient
{
public:
	WebStudentClient();
	~WebStudentClient();

	void Initial(std::string url);
	void SendWebMessage(std::string msg);
	void SendWebMessage(std::string, bool sleep_time);

private:
	CURL* curl_;
	CURLcode res_;
	char buf_[MAX_PATH];
};

#endif // !__WEB_STUDENT_CLIENT__