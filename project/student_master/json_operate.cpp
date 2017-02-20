#include "json_operate.h"

JsonOperate::JsonOperate()
{
}

JsonOperate::~JsonOperate()
{
}

const char * JsonOperate::AssembleJson(const StudentData &stu_data)
{
	//assert(stu_info_);
	using namespace rapidjson;
	assemble_json_str_ = "";

	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();

	Value root(kObjectType);

	Value studentname(kStringType);
	studentname.SetString(stu_data.appid_.c_str(), allocator);
	root.AddMember("appid", studentname, allocator);

	Value streamsource(kStringType);
	streamsource.SetString(stu_data.sno_.c_str(), allocator);
	root.AddMember("sno", streamsource, allocator);

	Value studentid(kStringType);
	studentid.SetString(stu_data.naem_.c_str(), allocator);
	root.AddMember("name", studentid, allocator);

	Value handup(kTrueType);
	handup.SetBool(stu_data.handup_);
	root.AddMember("handup", handup, allocator);

	Value operatetype(kNumberType);
	handup.SetInt(stu_data.operate_type_);
	root.AddMember("operatetype", operatetype, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> wtr(buffer);
	root.Accept(wtr);
	assemble_json_str_ = buffer.GetString();

	return assemble_json_str_.c_str();
}

void JsonOperate::JsonAnalysis(const char * stu_data, StudentData & log_info)
{
	assert(stu_data);
	using namespace rapidjson;
	Document doc;
	doc.Parse<0>(stu_data);

	log_info.appid_ = doc["name"].GetString();
	log_info.sno_ = doc["id"].GetString();
	log_info.naem_ = doc["ip"].GetString();
	log_info.handup_ = doc["handup"].GetBool();
	log_info.operate_type_ = OperateType(doc["operatetype"].GetInt());
}
