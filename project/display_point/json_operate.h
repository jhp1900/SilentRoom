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

enum OperateType {
	LOGON = 1,			// ��¼
	HANDUP = 2,			// ���ַ��ԣ�����ʦ�㲥�����ʹ��
	SPEAK = 3,			// ���ԣ�С������ʹ��
	STOP_SPEAK = 4,		// ��������
	KEEPA_LIVE = 5,		// 
};

/* ѧ��������Ϣ�ṹ�� */
struct StudentData
{
	std::string appid_;				// �����ţ������Ψһ��ʶ
	std::string sno_;				// ѧ��ѧ��
	std::string naem_;				// ѧ������
	bool handup_;					// ����״̬
	OperateType operate_type_;		// ��Ϣ����
	std::string stream_ip_;
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

