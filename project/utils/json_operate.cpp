#include "json_operate.h"
#include <wchar.h>
#include <Windows.h>

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
	operatetype.SetInt(stu_data.operate_type_);
	root.AddMember("operatetype", operatetype, allocator);

	Value stream_ip(kStringType);
	stream_ip.SetString(stu_data.stream_ip_.c_str(), allocator);
	root.AddMember("stream_ip", stream_ip, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> wtr(buffer);
	root.Accept(wtr);
	assemble_json_str_ = buffer.GetString();

	return assemble_json_str_.c_str();
}

const char * JsonOperate::AssembleJson(const LogonInfo & logon_info)
{
	using namespace rapidjson;

	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();

	Value root(kObjectType);

	Value appid(kStringType);
	appid.SetString(logon_info.appid.c_str(), allocator);
	root.AddMember("appid", appid, allocator);

	Value group_info(kStringType);
	group_info.SetString(logon_info.group_info.c_str(), allocator);
	root.AddMember("group_info", group_info, allocator);

	Value group_ip(kStringType);
	group_ip.SetString(logon_info.group_ip.c_str(), allocator);
	root.AddMember("group_ip", group_ip, allocator);

	StringBuffer buffer;
	Writer<StringBuffer> wtr(buffer);
	root.Accept(wtr);
	assemble_json_str_ = buffer.GetString();

	return assemble_json_str_.c_str();
}

const char * JsonOperate::AssembleJson(const std::vector<MasterData>& mast_dt)
{
	using namespace rapidjson;

	Document doc;
	Document::AllocatorType& allocator = doc.GetAllocator();
	Value json_array(kArrayType);

	for (auto &iter : mast_dt) {
		Value object(kObjectType);
		Value sno(kStringType);
		sno.SetString(iter.id.c_str(), allocator);
		object.AddMember("sno", sno, allocator);

		Value name(kStringType);
		name.SetString(iter.name.c_str(), allocator);
		object.AddMember("name", name, allocator);

		Value group_info(kStringType);
		group_info.SetString(iter.group_info.c_str(), allocator);
		object.AddMember("group_info", group_info, allocator);

		Value status(kFalseType);
		status.SetBool(iter.status);
		object.AddMember("status", status, allocator);

		json_array.PushBack(object, allocator);
	}

	StringBuffer buffer;
	Writer<StringBuffer> wtr(buffer);
	json_array.Accept(wtr);
	assemble_json_str_ = buffer.GetString();

	return assemble_json_str_.c_str();
}

void JsonOperate::JsonAnalysis(const char * stu_data, StudentData & log_info)
{
	assert(stu_data);
	using namespace rapidjson;
	Document doc;
	doc.Parse<0>(stu_data);

	log_info.appid_ = doc["appid"].GetString();
	log_info.sno_ = doc["sno"].GetString();
	log_info.naem_ = doc["name"].GetString();
	log_info.handup_ = doc["handup"].GetBool();
	log_info.operate_type_ = OperateType(doc["operatetype"].GetInt());
	log_info.stream_ip_ = doc["stream_ip"].GetString();
}

void JsonOperate::JsonAnalysis(const char * json_data, LogonInfo & logon_info)
{
	assert(json_data);
	using namespace rapidjson;
	Document doc;
	doc.Parse<0>(json_data);

	logon_info.appid = doc["appid"].GetString();
	logon_info.group_info = doc["group_info"].GetString();
	logon_info.group_ip = doc["group_ip"].GetString();
}
