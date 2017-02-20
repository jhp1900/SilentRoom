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
	std::string appid_;				// �����ţ������Ψһ��ʶ
	std::string sno_;				// ѧ��ѧ��
	std::string naem_;				// ѧ������
	bool handup_;					// ����״̬
	OperateType operate_type_;		// ��Ϣ����
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

