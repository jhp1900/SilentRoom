#pragma once

#include <iostream>
#include <rapidjson\rapidjson.h>
#include <rapidjson\stringbuffer.h>
#include <rapidjson\prettywriter.h>
#include <rapidjson\document.h>
//#include "common_define.h"

struct StudentData
{
	std::string stream_ip_;
	std::string group_info_;
	std::string student_name_;
	std::string student_id_;
	bool handup_;
};

class JsonOperate
{
public:
	JsonOperate();
	~JsonOperate();

	const char* AssembleJson(StudentData student_data);
	StudentData JsonAnalysis(const char* student_data);
private:
	StudentData analysis_json_struct_;
	std::string assemble_json_str_;
};

