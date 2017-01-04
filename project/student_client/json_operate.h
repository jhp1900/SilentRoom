#pragma once

#include <iostream>
#include <rapidjson\rapidjson.h>
#include <rapidjson\stringbuffer.h>
#include <rapidjson\prettywriter.h>
#include <rapidjson\document.h>
#include "common_define.h"

class JsonOperate
{
public:
	JsonOperate();
	~JsonOperate();

	bool SetStudentName(const char* student_name);
	bool SetStudentId(const char* student_id);
	bool SetStudentIp(const char* student_ip);
	bool SetStudentGroupInfo(const char* student_groupinfo);
	bool SetStudentHandupFlag(const char* student_handup_flag);

	virtual const char* AssembleJson(StudentData& student_data);
	virtual StudentData JsonAnalysis(const char* student_data);
private:
	StudentData student_data_;
	StudentData analysis_student_data_;
	std::string student_json_data_;
};

