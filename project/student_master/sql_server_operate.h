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
#include "..\utils\json_operate.h"

class MsSqlDbOperate
{
public:
	MsSqlDbOperate();
	~MsSqlDbOperate();

	int Connect(wchar_t* servername, wchar_t* username, wchar_t* password);
	int DisConnect();

	int ExecDirect(wchar_t* strsql);
	int CreateTable();

	int AddRecord(int student_id, wchar_t * student_name, int group_info, wchar_t* stream_ip, int handup);
	int DeleteRecord(wchar_t* student_name);
	int Update(wchar_t* sno, int handup);
	int Update(wchar_t* sno, wchar_t* group_info);

	LogonInfo* Query(wchar_t* in_appid);
	std::vector<MasterData>* QueryStatus();
private:
	LogonInfo longon_info_;
	std::vector<MasterData> master_data_;
	StudentData student_data_;
	SQLHENV henv_;
	SQLHDBC hdbc_;
	SQLHSTMT hstmt_;
};
#endif // !__SQL_SERVER_OPERATE_H__