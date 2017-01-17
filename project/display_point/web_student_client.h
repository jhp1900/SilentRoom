#ifndef __WEB_STUDENT_CLIENT__
#define __WEB_STUDENT_CLIENT__

#include <curl\curl.h>
#include <exception>
#include <string>

class WebStudentClient
{
public:
	WebStudentClient();
	~WebStudentClient();

	void Initial(std::string url);
	void SendWebMessage(std::string msg);

private:
	CURL* curl_;
	CURLcode res_;
};

#endif // !__WEB_STUDENT_CLIENT__