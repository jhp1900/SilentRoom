#pragma once
#include <string>

/*	获取本机IP地址
*	loc_ip: 输出参数，用于存储本机Ip
*/
void GetLocalIP(std::string &loc_ip);

/*	设置电脑为自动获取IP
*/
void SetAutoGetIP();