# SilentRoom

# 网页请求消息（请求都是 get 形式）
	一、键值说明：
		1、键 control: 操作类型 
			值 reload: 表示刷新，重新加载，这时需要查询所以相关信息并返回；
			值 speak: 表示请***发言；
			值 updata: 表示修复内容；
			值 add: 添加，可以添加分组IP（自然就添加了分组），也可以添加学生机
			值 delete: 删除，同 add;
			
		2、键 range: 操作范围
			值 students: 操作学生状态，一般用于刷新；
			值 group_mng: 操作分组信息，一般用于刷新、编辑；
			值 group_ip: 操作小组IP信息，一般用于刷新、编辑；
			值 to_group: 小组内发言；
			值 to_class: 班级发言;
			
		3、键 obj: 被操作的对象
				注：这里有可能是学号、小组、学生机编号（appid）
		
			
	二、示例
		1、刷新学生状态页：
			{control:"reload",range:"students"}
		
		2、请 2011号 同学在小组内发言
			{control:"speak",range:"to_group",obj:"2011"};
			