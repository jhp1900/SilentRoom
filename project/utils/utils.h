#pragma once
#include <string>

/*	��ȡ����IP��ַ
*	loc_ip: ������������ڴ洢����Ip
*/
void GetLocalIP(std::vector<std::wstring> &ips);

/*	���õ���Ϊ�Զ���ȡIP
*/
void SetAutoGetIP();

std::string UnicodeToUTF8(const wchar_t *str);
std::string UnicodeToUTF8(const char *str);

wchar_t* UTF8ToUnicode(const wchar_t *w_unicode);
wchar_t* UTF8ToUnicode(const char *unicode);

/*
*/
void ACP_UTF8(const char *str_in, char *str_out, int source, int target);