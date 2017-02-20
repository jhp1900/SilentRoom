#pragma once

#include <iostream>
#include <rapidjson\rapidjson.h>
#include <rapidjson\stringbuffer.h>
#include <rapidjson\prettywriter.h>
#include <rapidjson\document.h>
//#include "common_define.h"

/* ѧ��������Ϣ�ṹ�� */
struct StudentData
{
	std::string stream_ip_;
	std::string group_info_;
	std::string student_name_;
	std::string student_id_;
	bool handup_;
	int OperateType_;
	StudentData() {
		stream_ip_ = "";
		group_info_ = "";
		student_name_ = "";
		student_id_ = "";
		handup_ = false;
		OperateType_ = 0;
	}
};

/* ѧ����¼��Ϣ�ṹ�� */
typedef struct _LOGINFO
{
	std::string appid_;		// �����ţ������Ψһ��ʶ
	std::string sno_;		// ѧ��ѧ��
	std::string naem_;		// ѧ������
} LOGINFO;

class JsonOperate
{
public:
	JsonOperate();
	~JsonOperate();

	const char* AssembleJson(const StudentData &student_data);
	const char* AssembleJson(const LOGINFO &log_info);
	StudentData JsonAnalysis(const char* student_data);
	void JsonAnalysis(const char* stu_data, StudentData &log_info);
	void JsonAnalysis(const char* log_data, LOGINFO &log_info);

private:
	StudentData analysis_json_struct_;
	std::string assemble_json_str_;
};

