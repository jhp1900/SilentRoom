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
			std::cout << "initial sql enviroment handle failed" << std::endl;
			return -1;
		}

		retcode = SQLSetEnvAttr(henv_, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
			std::cout << "set sql enviroment faild" << std::endl;
			return -1;
		}

		retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv_, &hdbc_);
		if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
			std::cout << "initial sql handle faild" << std::endl;
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
			std::cout << "connect database faild" << std::endl;
			return -1;
		}
		std::cout << "connect database succeed" << std::endl;
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
		std::cout << "intial SQL_HANDLE_STMT faild" << std::endl;

		return -1;
	}

	retcode = SQLExecDirectW(hstmt_, (SQLWCHAR*)strsql, SQL_NTS);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
		std::cout << "exec sql faild" << std::endl;
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);

		return -1;
	}
	std::cout << "exec sql succeed" << std::endl;
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

StudentData* MsSqlDbOperate::Query(wchar_t* studentname)
{
	int ret = 0;
	wchar_t strsql[MAX_PATH];
	wchar_t student_name[MAX_PATH];
	wchar_t stream_ip[MAX_PATH];

	SQLINTEGER student_id, handup, group_info,operate_type;
	SQLINTEGER Cbstudent_id, Cbstudent_name, Cbgroup_info, Cbstream_ip, Cbhandup, Cboperate_type;

	wsprintfW(strsql, L"SELECT * FROM silentroom WHERE student_name='%s'", studentname);

	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt_);
	ret = SQLExecDirectW(hstmt_, (SQLWCHAR*)strsql, SQL_NTS);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
		return NULL;

	while ((ret = SQLFetch(hstmt_)) != SQL_NO_DATA)
	{
		SQLGetData(hstmt_, 1, SQL_C_LONG, &student_id, 0, &Cbstudent_id);
		SQLGetData(hstmt_, 2, SQL_C_WCHAR, student_name, 22, &Cbstudent_name);
		SQLGetData(hstmt_, 3, SQL_C_LONG, &group_info, 0, &Cbgroup_info);
		SQLGetData(hstmt_, 4, SQL_C_WCHAR, stream_ip, 256, &Cbstream_ip);
		SQLGetData(hstmt_, 5, SQL_C_LONG, &handup, 0, &Cbhandup);
		SQLGetData(hstmt_, 6, SQL_C_LONG, &operate_type, 0, &Cboperate_type);

		char char_student_id[MAX_PATH];
		char char_group_info[6];
		char char_handup[6];
		_itoa_s(student_id, char_student_id, 10);
		_itoa_s(group_info, char_group_info, 10);
		_itoa_s(handup, char_handup, 10);

		student_data_.student_id_ = char_student_id;
		student_data_.student_name_ = ATL::CW2A(student_name);
		student_data_.group_info_ = char_group_info;
		student_data_.stream_ip_ = ATL::CW2A(stream_ip);
		student_data_.handup_ = char_handup;
		student_data_.operateType_ = operate_type;
	}

	SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);

	return &student_data_;
}

//void MsSqlDbOperate::Query(wchar_t* studentname) {
//	int ret = 0;
//	wchar_t strsql[MAX_PATH];
//	wsprintfW(strsql, L"SELECT * FROM group_info WHERE appid='%s'", studentname);
//
//	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt_);
//	ret = SQLExecDirectW(hstmt_, (SQLWCHAR*)strsql, SQL_NTS);
//
//	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
//		return;
//}