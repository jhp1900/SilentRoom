#pragma once
#include <string>

/*	获取本机IP地址
*	loc_ip: 输出参数，用于存储本机Ip
*/
void GetLocalIP(std::vector<std::wstring> &ips);

/*	设置电脑为自动获取IP
*/
void SetAutoGetIP();

std::string UnicodeToUTF8(const wchar_t *str);
std::string UnicodeToUTF8(const char *str);

wchar_t* UTF8ToUnicode(const wchar_t *w_unicode);
wchar_t* UTF8ToUnicode(const char *unicode);

/*
*/
void ACP_UTF8(const char *str_in, char *str_out, int source, int target);