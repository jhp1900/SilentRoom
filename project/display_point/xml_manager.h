#pragma once
#include "stdafx.h"
#include "pugixml.hpp"

class XmlManager
{
public:
	XmlManager();
	~XmlManager();

public:
	BOOL IsLoad() const { return is_load_; }
	BOOL LoadFile(CDuiString file_path, CDuiString file_name);
	CDuiString GetNodeAttr(LPCTSTR nd_name, LPCTSTR attr_name);
	bool SetNodeAttr(LPCTSTR nd_name, LPCTSTR attr_name, LPCTSTR val);

public:
	char * WideToMulti(CDuiString wide, char * multi);    // ���ַ�ת���ֽ�
	CDuiString MultiToWide(string multi);                 // ���ֽ�ת���ַ� 

private:
	bool SaveFile();
	pugi::xml_node GetNode(const char * name);

private:
	pugi::xml_document file_;       // �����ļ�
	pugi::xml_node root_node_;      // �ļ����ڵ�
	CDuiString path_and_name_;      // ��·���ļ���
	BOOL is_load_;                  // �ļ�����״̬�� true ��ʾ���سɣ� false ��ʾ����ʧ�ܻ�û���ء�
};

