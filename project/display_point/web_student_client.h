#ifndef __WEB_STUDENT_CLIENT__
#define __WEB_STUDENT_CLIENT__

#include <curl\curl.h>
#include <exception>

class WebStudentClient
{
public:
	WebStudentClient();
	~WebStudentClient();

	void Initial();
	void SendWebMessage();

private:
	CURL* curl_;
	CURLcode res_;
};

#endif // !__WEB_STUDENT_CLIENT__