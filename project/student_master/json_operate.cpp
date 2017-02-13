#include "json_operate.h"

JsonOperate::JsonOperate()
{
}

JsonOperate::~JsonOperate()
{
}

const char * JsonOperate::AssembleJson(StudentData student_data)
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
	handup.SetInt(student_data.operateType_);
	root.AddMember("operatetype", operatetype, allocator);

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
