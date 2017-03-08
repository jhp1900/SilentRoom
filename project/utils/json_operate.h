#pragma once

#include <iostream>
#include <rapidjson\rapidjson.h>
#include <rapidjson\stringbuffer.h>
#include <rapidjson\prettywriter.h>
#include <rapidjson\document.h>
//#include "common_define.h"

struct LogonInfo
{
	std::string appid;
	std::string group_info;
	std::string group_ip;
};

struct MasterData {
	std::string id;
	std::string name;
	std::string group_info;
	bool status;
};

struct GroupManage
{
	std::string id;
	std::string group_info;
};

enum OperateType {
	NULL_TYPE = 0,
	LOGON = 1,				// 登录
	HANDUP = 2,				// 举手发言，在老师广播的情况使用
	SPEAK = 3,				// 发言，小组讨论使用
	STOP_SPEAK = 4,			// 结束发言
	KEEPA_LIVE = 5,			// 
	TEACHER_CONTROL = 6,	// 教师操控，教师确定使用哪个流
};

/* 学生发言信息结构体 */
struct StudentData
{
	std::string appid_;				// 程序编号，程序的唯一标识
	std::string sno_;				// 学生学号
	std::string naem_;				// 学生姓名
	bool handup_;					// 举手状态
	OperateType operate_type_;		// 信息类型
	std::string stream_ip_;			// 切换用的流地址

	bool operator==(const StudentData value) {
		return(appid_ == value.appid_ && sno_ == value.sno_
			&& naem_ == value.naem_
			&& handup_ == value.handup_
			&& operate_type_ == value.operate_type_
			&& stream_ip_ == value.stream_ip_);
	}

	StudentData& operator=(StudentData& value) {
		appid_ = value.appid_;
		sno_ = value.sno_;
		naem_ = value.naem_;
		handup_ = value.handup_;
		operate_type_ = value.operate_type_;
		stream_ip_ = value.stream_ip_;
		return *this;
	}
};

class JsonOperate
{
public:
	JsonOperate();
	~JsonOperate();

	const char* AssembleJson(const StudentData &stu_data);
	const char* AssembleJson(const LogonInfo &logon_info);
	void JsonAnalysis(const char* json_data, StudentData &stu_data);
	void JsonAnalysis(const char* json_data, LogonInfo &logon_info);

private:
	StudentData analysis_json_struct_;
	std::string assemble_json_str_;
};

