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
	std::string appid_;				// 程序编号，程序的唯一标识
	std::string sno_;				// 学生学号
	std::string naem_;				// 学生姓名
	bool handup_;					// 举手状态
	OperateType operate_type_;		// 信息类型
};

class JsonOperate
{
public:
	JsonOperate();
	~JsonOperate();

	const char* AssembleJson(const StudentData &stu_data);
	void JsonAnalysis(const char* json_data, StudentData &stu_data);
private:
	std::string assemble_json_str_;
};

