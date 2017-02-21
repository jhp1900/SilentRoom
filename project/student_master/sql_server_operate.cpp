#include "sql_server_operate.h"

MsSqlDbOperate::MsSqlDbOperate()
{
}

MsSqlDbOperate::~MsSqlDbOperate()
{
}

int MsSqlDbOperate::Connect(wchar_t * servername, wchar_t * username, wchar_t * password)
{
	try
	{
		SQLRETURN retcode;
		retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv_);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
#ifdef DEBUG
			OutputDebugStringA("initial sql enviroment handle failed \n");
#endif // DEBUG
			return -1;
		}

		retcode = SQLSetEnvAttr(henv_, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
#ifdef DEBUG
			OutputDebugStringA("set sql enviroment faild \n");
#endif // DEBUG
			return -1;
		}

		retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv_, &hdbc_);
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
#ifdef DEBUG
			OutputDebugStringA("initial sql handle faild \n");
#endif // DEBUG
			return -1;
		}

		retcode = SQLConnectW(hdbc_,
			(SQLWCHAR*)servername,
			SQL_NTS,
			(SQLWCHAR*)username,
			SQL_NTS,
			(SQLWCHAR*)password,
			SQL_NTS);

		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
#ifdef DEBUG
			OutputDebugStringA("connect database faild \n");
#endif // DEBUG
			return -1;
		}
#ifdef DEBUG
		OutputDebugStringA("connect database succeed \n");
#endif // DEBUG
		return 0;

	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}

int MsSqlDbOperate::DisConnect()
{
	SQLDisconnect(hdbc_);

	SQLFreeHandle(SQL_HANDLE_DBC, hdbc_);
	SQLFreeHandle(SQL_HANDLE_ENV, henv_);

	return 0;
}

int MsSqlDbOperate::ExecDirect(wchar_t * strsql)
{
	SQLRETURN retcode;

	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt_);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
#ifdef DEBUG
		OutputDebugStringA("intial SQL_HANDLE_STMT faild \n");
#endif // DEBUG

		return -1;
	}

	retcode = SQLExecDirectW(hstmt_, (SQLWCHAR*)strsql, SQL_NTS);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
#ifdef DEBUG
		OutputDebugStringA("exec sql faild \n");
#endif // DEBUG
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);

		return -1;
	}
#ifdef DEBUG
	OutputDebugStringA("exec sql succeed \n");
#endif // DEBUG
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);

	return 0;
}

int MsSqlDbOperate::CreateTable()
{
	int ret = 0;

	wchar_t* strsql = L"CREATE TABLE silentroom(student_id INT NOT NULL, student_name VARCHAR(10), group_info INT, stream_ip VARCHAR(128), handup int PRIMARY KEY(student_id))";
	ret = ExecDirect(strsql);
	if (ret == -1)
		return -1;
	return 0;
}

int MsSqlDbOperate::AddRecord(int student_id, wchar_t * student_name, int group_info, wchar_t* stream_ip, int handup)
{
	int ret = 0;
	wchar_t strsql[MAX_PATH];
	wsprintfW(strsql, L"INSERT INTO silentroom(student_id,student_name,group_info,stream_ip,handup) VALUES(%d, '%s', %d, '%s', %d);",
		student_id, student_name, group_info, stream_ip, handup);
	ret = ExecDirect(strsql);
	if (ret == -1)
		return -1;
	return 0;
}

int MsSqlDbOperate::DeleteRecord(wchar_t * student_name)
{
	int ret = 0;
	wchar_t strsql[MAX_PATH];
	wsprintfW(strsql, L"DELETE FROM silentroom WHERE student_name='%s'", student_name);
	if (ret = ExecDirect(strsql) == -1)
		return -1;
	return 0;
}

LogonInfo* MsSqlDbOperate::Query(wchar_t* in_appid)
{
	int ret = 0;
	wchar_t strsql[MAX_PATH];
	wchar_t appid[MAX_PATH];
	wchar_t group_info[MAX_PATH];
	wchar_t group_ip[MAX_PATH];

	//SQLINTEGER student_id, handup, group_info,operate_type;
	SQLINTEGER Cbappid, Cbgroup_info, Cbgroup_ip;

	wsprintfW(strsql, L"SELECT * FROM group_info WHERE appid='%s'", in_appid);

	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt_);
	ret = SQLExecDirectW(hstmt_, (SQLWCHAR*)strsql, SQL_NTS);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
		return NULL;

	while ((ret = SQLFetch(hstmt_)) != SQL_NO_DATA)
	{
		SQLGetData(hstmt_, 1, SQL_C_WCHAR, appid, 128, &Cbappid);
		SQLGetData(hstmt_, 2, SQL_C_WCHAR, group_info, 128, &Cbgroup_info);
		SQLGetData(hstmt_, 3, SQL_C_WCHAR, group_ip, 256, &Cbgroup_ip);
		longon_info_.appid = ATL::CW2A(appid);
		longon_info_.group_info = ATL::CW2A(group_info);
		longon_info_.group_ip = ATL::CW2A(group_ip);

	}

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);

	return &longon_info_;
}