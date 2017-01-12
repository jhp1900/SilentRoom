#include "json_operate.h"



JsonOperate::JsonOperate()
{
}


JsonOperate::~JsonOperate()
{
}

//bool JsonOperate::SetStudentName(const char * student_name)
//{
//	assert(student_name);
//	student_data_.student_name_ = student_name;
//	return true;
//}
//
//bool JsonOperate::SetStudentId(const char * student_id)
//{
//	assert(student_id);
//	student_data_.student_id_ = student_id;
//	return true;
//}
//
//bool JsonOperate::SetStudentIp(const char * student_ip)
//{
//	assert(student_ip);
//	student_data_.stream_ip_ = student_ip;
//	return true;
//}
//
//bool JsonOperate::SetStudentGroupInfo(const char * student_groupinfo)
//{
//	assert(student_groupinfo);
//	student_data_.group_info_ = student_groupinfo;
//	return true;
//}
//
//bool JsonOperate::SetStudentHandupFlag(const char * student_handup_flag)
//{
//	assert(student_handup_flag);
//	student_data_.handup_ = student_handup_flag;
//	return true;
//}

const char * JsonOperate::AssembleJson(StudentData student_data)
{
	//assert(student_data);
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

	return analysis_json_struct_;
}
