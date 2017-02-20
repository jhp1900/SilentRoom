#include "json_operate.h"
#include <wchar.h>
#include <Windows.h>

JsonOperate::JsonOperate()
{
}

JsonOperate::~JsonOperate()
{
}

const char * JsonOperate::AssembleJson(const StudentData &student_data)
{
	//assert(student_info_);
	using namespace rapidjson;
	assemble_json_str_ = "";

	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();

	Value root(kObjectType);

	Value studentname(kStringType);
	studentname.SetString(student_data.student_name_.c_str(), allocator);
	root.AddMember("name", studentname, allocator);

	Value streamsource(kStringType);
	streamsource.SetString(student_data.stream_ip_.c_str(), allocator);
	root.AddMember("ip", streamsource, allocator);

	Value studentid(kStringType);
	studentid.SetString(student_data.student_id_.c_str(), allocator);
	root.AddMember("id", studentid, allocator);

	Value groupinfo(kStringType);
	groupinfo.SetString(student_data.group_info_.c_str(), allocator);
	root.AddMember("group", groupinfo, allocator);

	Value handup(kTrueType);
	handup.SetBool(student_data.handup_);
	root.AddMember("handup", handup, allocator);

	Value operatetype(kNumberType);
	handup.SetInt(student_data.OperateType_);
	root.AddMember("operatetype", operatetype, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> wtr(buffer);
	root.Accept(wtr);
	assemble_json_str_ = buffer.GetString();

	return assemble_json_str_.c_str();
}

const char * JsonOperate::AssembleJson(const LOGINFO & log_info)
{
	using namespace rapidjson;
	assemble_json_str_ = "";

	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();

	Value root(kObjectType);

	Value log_appid(kStringType);
	log_appid.SetString(log_info.appid_.c_str(), allocator);
	root.AddMember("appid", log_appid, allocator);

	Value log_sno(kStringType);
	log_sno.SetString(log_info.sno_.c_str(), allocator);
	root.AddMember("sno", log_sno, allocator);

	Value log_name(kStringType);
	log_name.SetString(log_info.naem_.c_str(), allocator);
	root.AddMember("name", log_name, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> wtr(buffer);
	root.Accept(wtr);
	assemble_json_str_ = buffer.GetString();

	return assemble_json_str_.c_str();
}

StudentData JsonOperate::JsonAnalysis(const char * student_data)
{
	assert(student_data);
	using namespace rapidjson;
	Document doc;
	doc.Parse<0>(student_data);

	memset(&analysis_json_struct_, 0, sizeof(StudentData));

	analysis_json_struct_.student_name_ = doc["name"].GetString();
	analysis_json_struct_.student_id_ = doc["id"].GetString();
	analysis_json_struct_.stream_ip_ = doc["ip"].GetString();
	analysis_json_struct_.group_info_ = doc["group"].GetString();
	analysis_json_struct_.handup_ = doc["handup"].GetBool();
	analysis_json_struct_.OperateType_ = doc["operatetype"].GetInt();

	return analysis_json_struct_;
}

void JsonOperate::JsonAnalysis(const char * stu_data, StudentData & log_info)
{
	assert(stu_data);
	using namespace rapidjson;
	Document doc;
	doc.Parse<0>(stu_data);

	log_info.student_name_ = doc["name"].GetString();
	log_info.student_id_ = doc["id"].GetString();
	log_info.stream_ip_ = doc["ip"].GetString();
	log_info.group_info_ = doc["group"].GetString();
	log_info.handup_ = doc["handup"].GetBool();
	log_info.OperateType_ = doc["operatetype"].GetInt();
}

void JsonOperate::JsonAnalysis(const char * log_data, LOGINFO & log_info)
{
	assert(log_data);
	using namespace rapidjson;
	Document doc;
	doc.Parse<0>(log_data);

	log_info.appid_ = doc["appid"].GetString();
	log_info.sno_ = doc["sno"].GetString();
	log_info.naem_ = doc["name"].GetString();
}
