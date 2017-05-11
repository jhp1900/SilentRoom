#include "xml_manager.h"
#include "pugixml.cpp"
#include <io.h>
#include <atlbase.h>

XmlManager::XmlManager() :
	path_and_name_(_T("")),
	is_load_(false)
{
}

XmlManager::~XmlManager()
{
}

BOOL XmlManager::LoadFile(CDuiString file_path, CDuiString file_name)
{
	path_and_name_ = file_path + file_name;
	char path[MAX_PATH] = { 0 };

	/* 如果文件不存在，则创建一个配置文件格式的文件; */
	if (_access(WideToMulti(path_and_name_, path), 0) == -1) {
		HANDLE file_handle = CreateFile(path_and_name_, GENERIC_WRITE | GENERIC_READ,
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		DWORD file_size = 0;
		CDuiString buffer = _T("<?xml version=\"1.0\" encoding=\"utf - 8\"?><Info></Info>");
		::WriteFile(file_handle, buffer, buffer.GetLength() * 2, &file_size, nullptr);

		CloseHandle(file_handle);   // 关闭文件
	}

	is_load_ = file_.load_file(path_and_name_.GetData());
	return is_load_;
}

CDuiString XmlManager::GetNodeAttr(LPCTSTR nd_name, LPCTSTR attr_name)
{
	char temp[MAX_PATH] = { 0 };
	pugi::xml_node node = GetNode(CW2A(nd_name));
	pugi::xml_attribute attr = node.attribute(CW2A(attr_name));
	if (!attr) {
		attr = node.append_attribute(CW2A(attr_name));
		attr = "";
		SaveFile();
	}
	return *attr.as_string();
}

bool XmlManager::SetNodeAttr(LPCTSTR nd_name, LPCTSTR attr_name, LPCTSTR val)
{
	char temp[MAX_PATH] = { 0 };
	pugi::xml_node node = GetNode(CW2A(nd_name));
	pugi::xml_attribute attr = node.attribute(CW2A(attr_name));
	if (!attr)
		attr = node.append_attribute(CW2A(attr_name));
	attr = CW2A(val);

	return SaveFile();
}

/* （工具函数） 宽字节 转 多字节 */
char * XmlManager::WideToMulti(CDuiString wide, char * multi)
{
	ZeroMemory(multi, MAX_PATH);
	int wide_len = wide.GetLength();
	int multi_len = WideCharToMultiByte(CP_ACP, NULL, wide.GetData(), wide_len, NULL, 0, NULL, FALSE);
	::WideCharToMultiByte(CP_ACP, NULL, wide.GetData(), -1, multi, multi_len, NULL, FALSE);
	return multi;
}

/* （工具函数） 多字节 转 宽字节 */
CDuiString XmlManager::MultiToWide(string multi)
{
	int multi_len = multi.length();
	int wide_len = ::MultiByteToWideChar(CP_ACP, 0, multi.c_str(), -1, NULL, 0);
	wchar_t *wide_str;
	wide_str = new wchar_t[wide_len + 1];
	memset(wide_str, 0, (wide_len + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, multi.c_str(), -1, (LPWSTR)wide_str, wide_len);

	CDuiString ret = (wchar_t*)wide_str;
	delete wide_str;

	return ret;
}

bool XmlManager::SaveFile()
{
	return file_.save_file(path_and_name_.GetData(), "  ", pugi::format_indent | pugi::format_write_bom, pugi::encoding_utf8);
}

pugi::xml_node XmlManager::GetNode(const char * name)
{
	pugi::xml_node node = file_.child("Info").child(name);
	if (!node)
		node = file_.child("Info").append_child(name);
	return node;
}
