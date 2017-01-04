#include "json_operate.h"



JsonOperate::JsonOperate()
{
}


JsonOperate::~JsonOperate()
{
}

bool JsonOperate::SetStudentName(const char * student_name)
{
	assert(student_name);
	student_data_.student_name_ = student_name;
	return true;
}

bool JsonOperate::SetStudentId(const char * student_id)
{
	assert(student_id);
	student_data_.student_id_ = student_id;
	return true;
}

bool JsonOperate::SetStudentIp(const char * student_ip)
{
	assert(student_ip);
	student_data_.stream_ip_ = student_ip;
	return true;
}

bool JsonOperate::SetStudentGroupInfo(const char * student_groupinfo)
{
	assert(student_groupinfo);
	student_data_.group_info_ = student_groupinfo;
	return true;
}

bool JsonOperate::SetStudentHandupFlag(const char * student_handup_flag)
{
	assert(student_handup_flag);
	student_data_.handup_ = student_handup_flag;
	return true;
}

const char * JsonOperate::AssembleJson(StudentData & student_data)
{
	assert(&student_data);
	using namespace rapidjson;

	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();

	Value root(kObjectType);

	Value studentname(kStringType);
	studentname.SetString(student_data_.student_name_.c_str(), allocator);
	root.AddMember("studentname", studentname, allocator);

	Value streamsource(kStringType);
	streamsource.SetString(student_data_.stream_ip_.c_str(), allocator);
	root.AddMember("streamip", streamsource, allocator);

	Value studentid(kStringType);
	studentid.SetString(student_data_.student_id_.c_str(), allocator);
	root.AddMember("studentid", studentid, allocator);

	Value groupinfo(kStringType);
	groupinfo.SetString(student_data_.group_info_.c_str(), allocator);
	root.AddMember("groupinfo", groupinfo, allocator);

	Value handup(kStringType);
	handup.SetString(student_data_.handup_.c_str(), allocator);
	root.AddMember("handup", handup, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> wtr(buffer);
	root.Accept(wtr);
	std::string reststring = buffer.GetString();
	student_json_data_ = buffer.GetString();

	return student_json_data_.c_str();
}

StudentData JsonOperate::JsonAnalysis(const char * student_data)
{
	assert(student_data);
	using namespace rapidjson;
	Document doc;
	doc.Parse<0>(student_json_data_.c_str());

	analysis_student_data_.student_name_ = doc["studentname"].GetString();
	analysis_student_data_.student_id_ = doc["studentid"].GetString();
	analysis_student_data_.stream_ip_ = doc["studentip"].GetString();
	analysis_student_data_.group_info_ = doc["groupinfo"].GetString();
	analysis_student_data_.handup_ = doc["handup"].GetString();

	return analysis_student_data_;
}
