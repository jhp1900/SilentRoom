#pragma once
#include <sqlite3.h>
#include <string>

class SQLiteTool
{
public:
	SQLiteTool();
	~SQLiteTool();

	static int exec_callback(void *not_used, int argc, char **argv, char **col_name);
	int OpenDB(std::string db_name);
	int CloseDB();
	int CreateTable(std::string sql);
	int Select(
		std::string sql,
		void *data = nullptr,
		int(*callback)(void*, int, char**, char**) = exec_callback
	);

private:
	int SQLExec(
		std::string sql,
		void *data = nullptr,
		int(*callback)(void*, int, char**, char**) = exec_callback
	);

private:
	sqlite3 *db_;
	char *error_msg_;
};
