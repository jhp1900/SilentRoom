#ifndef __WEB_STUDENT_CLIENT__
#define __WEB_STUDENT_CLIENT__

#include <curl\curl.h>
#include <exception>
#include <string>
#include <thread>

class WebStudentClient
{
public:
	WebStudentClient();
	~WebStudentClient();

	void Initial(std::string url);
	void SendWebMessage(std::string msg);
	void KeepAlive(std::string);

private:
	CURL* curl_;
	CURLcode res_;
	char buf_[MAX_PATH];
};

#endif // !__WEB_STUDENT_CLIENT__