#include "sql_server_operate.h"
#include <locale.h>

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
	//int ret = 0;
	//wchar_t* strsql = L"CREATE TABLE silentroom(student_id INT NOT NULL, student_name VARCHAR(10), group_info INT, stream_ip VARCHAR(128), handup int PRIMARY KEY(student_id))";
	//ret = ExecDirect(strsql);
	//if (ret == -1)
	//	return -1;
	return 0;
}

//添加学生
int MsSqlDbOperate::AddStudent(wchar_t* student_id, wchar_t * student_name, wchar_t* appid)
{
	int ret = 0;
	wchar_t strsql[MAX_PATH];
	wsprintfW(strsql, L"INSERT INTO student_info(student_id,student_name,appid) VALUES('%s', '%s', '%s');",
		student_id, student_name, appid);
	ret = ExecDirect(strsql);
	if (ret == -1)
		return -1;
	return 0;
}

//删除学生
int MsSqlDbOperate::DeleteStudent(wchar_t * sno)
{
	int ret = 0;
	wchar_t strsql[MAX_PATH];
	wsprintfW(strsql, L"DELETE FROM student_info WHERE sno='%s'", sno);
	if (ret = ExecDirect(strsql) == -1)
		return -1;
	return 0;
}

//学生表发言操作
int MsSqlDbOperate::Update(wchar_t * sno, int handup)
{
	int ret = 0;
	wchar_t strsql[MAX_PATH];
	wsprintfW(strsql, L"UPDATE student_info SET handup='%d' WHERE sno='%s'", handup, sno);
	if (ExecDirect(strsql) == -1)
		return -1;
	return 0;
}

//分组管理操作
int MsSqlDbOperate::Update(wchar_t * sno, wchar_t * group_info)
{
	int ret = 0;
	wchar_t strsql[MAX_PATH];
	wsprintfW(strsql, L"UPDATE group_info SET group_info = '%s' WHERE appid = (SELECT appid FROM student_info WHERE sno = '%s')", sno, group_info);
	return ExecDirect(strsql) ? 0 : -1;
}

//修改分组IP
int MsSqlDbOperate::UpdateGroupIp(wchar_t * group_ip, wchar_t * group)
{
	int ret = 0;
	wchar_t strsql[MAX_PATH];
	wsprintfW(strsql, L"UPDATE group_info SET group_ip = '%s' WHERE group_info = '%s'", group_ip, group);
	return ExecDirect(strsql) ? 0 : -1;
}

//学生登陆操作
LogonInfo* MsSqlDbOperate::Query(wchar_t* in_appid)
{
	int ret = 0;
	wchar_t strsql[MAX_PATH];
	wchar_t appid[MAX_PATH];
	wchar_t group_info[MAX_PATH];
	wchar_t group_ip[MAX_PATH];

	//SQLINTEGER student_id, handup, group_info,operate_type;
	SQLINTEGER Cbappid, Cbgroup_info, Cbgroup_ip;

	wsprintfW(strsql, L"SELECT appid, g.group_info, group_ip FROM group_info g inner join group_ip p on g.group_info = p.group_info WHERE appid='%s'", in_appid);

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

//学生状态查询
std::vector<MasterData>* MsSqlDbOperate::QueryStatus()
{
	int ret = 0;

	wchar_t strsql[MAX_PATH];
	wchar_t id[MAX_PATH];
	wchar_t student_name[MAX_PATH];
	wchar_t group_info[MAX_PATH];
	char ttname[MAX_PATH];
	int handup;
	std::vector<MasterData> master_tmp;

	SQLINTEGER Cbid, Cbname, Cbgroup_info, Cbhandup;

	wsprintfW(strsql, L"select sno,name,group_info,handup from student_info s inner join group_info g on s.appid = g.appid");
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt_);
	ret = SQLExecDirectW(hstmt_, (SQLWCHAR*)strsql, SQL_NTS);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
		return NULL;
	while ((ret = SQLFetch(hstmt_)) != SQL_NO_DATA) {
		SQLGetData(hstmt_, 1, SQL_C_WCHAR, id, 128, &Cbid);
		SQLGetData(hstmt_, 2, SQL_C_WCHAR, student_name, 50, &Cbname);
		SQLGetData(hstmt_, 3, SQL_C_WCHAR, group_info, 256, &Cbgroup_info);
		SQLGetData(hstmt_, 4, SQL_C_LONG, &handup, 256, &Cbhandup);
		MasterData master_data_tmp;

		master_data_tmp.id = ATL::CW2A(id);
		master_data_tmp.name = ATL::CW2A(student_name);
		master_data_tmp.group_info = ATL::CW2A(group_info);
		master_data_tmp.status = handup;

		master_tmp.push_back(master_data_tmp);
		
	}
	master_data_ = master_tmp;
	return &master_data_;
}
