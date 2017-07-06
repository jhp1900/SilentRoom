# SilentRoom

# 网页请求消息（请求都是 get 形式）
	control:to_class  		请到班级发言
	control:teacher_speak  	老师讲课
	control:auto_speak		自由讨论
	
	obj:	发言者学号
	
# netsh 设置 ip、dns 以及 自动获取的示例：
	netsh interface ip set address name="WLAN" static 18.18.8.27 255.255.255.0 18.18.8.1 
	netsh interface ip set dnsservers name="WLAN" static 61.139.2.69
	netsh interface ip set address name="WLAN" dhcp 
	netsh interface ip set dnsservers name="WLAN" dhcp