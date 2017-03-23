#ifndef __SQL_SERVER_OPERATE_H__
#define __SQL_SERVER_OPERATE_H__

#include <Windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <odbcss.h>
#include <iostream>
#include <atlconv.h>
#include <atlbase.h>
#include <atlstr.h>
#include <vector>
#include <list>
#include <mutex>
#include "..\utils\json_operate.h"

class MsSqlDbOperate
{
public:
	MsSqlDbOperate();
	~MsSqlDbOperate();

	int Connect(const wchar_t* servername, const wchar_t* username, const wchar_t* password);
	int DisConnect();

	int ExecDirect(wchar_t* strsql);
	int CreateTable();

	int AddStudent(const wchar_t* sno, const wchar_t * name, const wchar_t* appid
		, const int handup, const wchar_t* stream_ip);
	int DeleteStudent(wchar_t* sno);

	int AddAppid(wchar_t* appid, wchar_t* group);
	int DeleteAppid(wchar_t* appid);
	int UpdateAppid(wchar_t* appid, wchar_t* group);

	int AddGroupIp(wchar_t* group, wchar_t* group_ip);
	int DeleteGroupIp(wchar_t* group);
	int UpdateGroupIp(wchar_t* group_ip, wchar_t* group_info);

	int DeleteGroup(wchar_t* group);
	int Update(wchar_t* sno, int handup);
	int Update(wchar_t* sno, wchar_t* group_info);

	std::wstring QueryStudentIp(wchar_t* sno);
	bool Query(wchar_t* in_appid, LogonInfo &lg_info);
	std::vector<MasterData>* QueryStatus();
	std::vector<GroupManage>* QueryGroupManager();
	std::vector<GroupIP>* QueryGroupIP();

public:
	std::mutex mutex_;
	LogonInfo longon_info_;
	std::vector<MasterData> master_data_;
	std::vector<GroupManage> group_manager_;
	std::vector<GroupIP> group_ip_;
	StudentData student_data_;
	SQLHENV henv_;
	SQLHDBC hdbc_;
	SQLHSTMT hstmt_;
};
#endif // !__SQL_SERVER_OPERATE_H__