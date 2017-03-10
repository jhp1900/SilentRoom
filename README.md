# SilentRoom

# 网页请求消息（请求都是 get 形式）
	一、键值说明：
		1、键 control: 操作类型 
			值 reload: 表示刷新，重新加载，这时需要查询所以相关信息并返回；
			值 speak: 表示请***发言；
			值 updata: 表示修复内容；
			
		2、键 range: 操作范围
			值 students: 操作学生状态，一般用于刷新；
			值 group_mng: 操作分组信息，一般用于刷新、编辑；
			值 group_ip: 操作小组IP信息，一般用于刷新、编辑；
			值 to_group: 小组内发言；
			值 to_class: 班级发言;
			
		3、键 sno: 被操作学生的学号
		
		4、键 obj_group: 被操作的小组
		
		5、键 appid: 被操作的学生机编号（优盘编号）
			
	二、示例
		1、刷新学生状态页：
			{control:"reload",range:"students"}
		
		2、请 2011号 同学在小组内发言
			{control:"speak",range:"to_group",sno:"2011"};
			