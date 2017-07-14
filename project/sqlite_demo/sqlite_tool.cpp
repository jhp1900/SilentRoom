#include "sqlite_tool.h"
#include <cstdio>

SQLiteTool::SQLiteTool()
{
}

SQLiteTool::~SQLiteTool()
{
	if (db_)
		CloseDB();
}

int SQLiteTool::exec_callback(void * not_used, int argc, char ** argv, char ** col_name)
{
	for (int i = 0; i < argc; ++i)
		printf("%s = %s\n", col_name[i], argv[i] ? argv[i] : "NULL");
	printf("\n");
	return 0;
}

int SQLiteTool::OpenDB(std::string db_name)
{
	int ret = SQLITE_OK;
	if (!db_) {
		if ((ret = sqlite3_open(db_name.c_str(), &db_)) == SQLITE_OK) {
			fprintf(stderr, "Opened database successfully!\n");
		}
		else {
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_));
			CloseDB();
		}
	}
	return ret;
}

int SQLiteTool::CloseDB()
{
	int ret = SQLITE_OK;
	if ((ret = sqlite3_close(db_)) == SQLITE_OK) {
		db_ = nullptr;
		fprintf(stderr, "Close database successfully!\n");
	}

	return ret;
}

int SQLiteTool::CreateTable(std::string sql)
{
	return SQLExec(sql);
}

int SQLiteTool::Select(
	std::string sql,
	void *data /* = nullptr */,
	int(*callback)(void*, int, char**, char**) /* = exec_callback */
	)
{
	return SQLExec(sql, data, callback);
}

int SQLiteTool::SQLExec(
	std::string sql,
	void * data /* = nullptr */,
	int(*callback)(void*, int, char**, char**) /* = exec_callback */
	)
{
	int ret = sqlite3_exec(db_, sql.c_str(), callback, data, &error_msg_);
	if (ret == SQLITE_OK)
		fprintf(stderr, "SQLExec is OK!\n");
	else {
		fprintf(stderr, "SQLExec is Error: %s", sqlite3_errmsg(db_));
		sqlite3_free(error_msg_);
	}
	return ret;
}
