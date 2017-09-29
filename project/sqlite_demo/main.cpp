#include <iostream>
#include "sqlite_tool.h"
#include <vector>
#include <map>

using std::vector;
using std::map;
using std::string;

int exec_callback(void * data, int argc, char ** argv, char ** col_name)
{
	if (!data)
		return 0;
	auto t = static_cast<vector<map<string, string>>*>(data);
	map<string, string> t_map;
	for (int i = 0; i < argc; ++i) {
		t_map[col_name[i]] = argv[i] ? argv[i] : "";
	}
	(*t).push_back(t_map);

	for (int i = 0; i < argc; ++i)
		printf("%s = %s --- da da da ...\n", col_name[i], argv[i] ? argv[i] : "NULL");
	printf("\n");
	return 0;
}

int main()
{
	//SQLiteTool sql_tool;
	//int ret = sql_tool.OpenDB("test_db.db");
	////if (ret == SQLITE_OK) {
	////	std::string sql = "create table demo_tab("\
	////		"id int primary key not null,"\
	////		"name text not null,"\
	////		"addr text);";
	////	ret = sql_tool.CreateTable(sql);
	////}
	//if (ret == SQLITE_OK) {
	//	std::string sql = "SELECT demo_tab.id,name,handup,demo_tab2.'group' FROM demo_tab inner join demo_tab2 on demo_tab.id = demo_tab2.id;";
	//	vector<map<string, string>> data;
	//	sql_tool.Select(sql, &data, exec_callback);
	//	for (auto i_map : data) {
	//		for(auto item : i_map)
	//			printf("%s = %s --- da da da ...\n", item.first.c_str(), item.second.c_str());
	//		printf("\n");
	//	}
	//}
	//sql_tool.CloseDB();

	char sql[_MAX_PATH] = { 0 };
	string name = "name";
	string addr = "addr";
	int sno = 123;
	sprintf_s(sql, "test %s, %d, %s", name.c_str(), sno, addr.c_str());
	
	printf(sql);

	system("pause");
	return 0;
}