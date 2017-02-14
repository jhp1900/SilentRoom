#pragma once

#include <iostream>
#include <rapidjson\rapidjson.h>
#include <rapidjson\stringbuffer.h>
#include <rapidjson\prettywriter.h>
#include <rapidjson\document.h>
//#include "common_define.h"

enum OperateType {
	logon = 1,
	handup = 2,
	keepalive = 3
};

struct StudentData
{
	std::string stream_ip_;
	std::string group_info_;
	std::string student_name_;
	std::string student_id_;
	bool handup_;
	int operateType_;
	StudentData() {
		stream_ip_ = nullptr;
		group_info_ = nullptr;
		student_name_ = nullptr;
		student_id_ = nullptr;
		handup_ = false;
		operateType_ = 0;
	}
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

