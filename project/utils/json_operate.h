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
	LOGON = 1,				// ��¼
	HANDUP = 2,				// ���ַ��ԣ�����ʦ�㲥�����ʹ��
	SPEAK = 3,				// ���ԣ�С������ʹ��
	STOP_SPEAK = 4,			// ��������
	KEEPA_LIVE = 5,			// 
	TEACHER_CONTROL = 6,	// ��ʦ�ٿأ���ʦȷ��ʹ���ĸ���
};

/* ѧ��������Ϣ�ṹ�� */
struct StudentData
{
	std::string appid_;				// �����ţ������Ψһ��ʶ
	std::string sno_;				// ѧ��ѧ��
	std::string naem_;				// ѧ������
	bool handup_;					// ����״̬
	OperateType operate_type_;		// ��Ϣ����
	std::string stream_ip_;			// �л��õ�����ַ

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

