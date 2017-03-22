/*
* 数据库操作
*
*/
function DBManager(){
	this.db_connect=function(){
		var conn = new ActiveXObject("ADODB.Connection");
		conn.Open("Provider=SQLOLEDB.1;Data Source=10.18.3.67,1433;User ID=sa;Password=123;Initial Catalog=demo");
		return conn;
	}

	this.reload_stu=function(){
		var conn = this.db_connect();
		var rs;
		var json_arry = [];
		try{
			rs = new ActiveXObject("ADODB.Recordset");
			var sql = "select * from student_info join group_info on(student_info.appid = group_info.appid)";
			rs.open(sql, conn);
			while(!rs.EOF){
				var temp = {"sno":rs.Fields("sno").Value};
				temp.name = rs.Fields("name").Value;
				temp.group_info = rs.Fields("group_info").Value;
				temp.handup = rs.Fields("handup").Value;
				json_arry.push(temp);
				rs.moveNext();
			}
		}catch(e){
			document.write(e.description);
		}finally{
			this.db_closeAll(rs, conn);
		}
		return json_arry;
	}

	this.reload_ginfo=function(){
		var conn = this.db_connect();
		var rs;
		var json_arry = [];
		try{
			rs = new ActiveXObject("ADODB.Recordset");
			var sql = "select * from group_info";
			rs.open(sql, conn);
			while(!rs.EOF){
				json_arry.push({"appid":rs.Fields("appid").Value, "group_info":rs.Fields("group_info").Value});
				rs.moveNext();
			}
		}catch(e){
			document.write(e.description);
		}finally{
			this.db_closeAll(rs, conn);
		}
		return json_arry;
	}

	this.reload_gip=function(){
		var conn = this.db_connect();
		var rs;
		var json_arry = [];
		try{
			rs = new ActiveXObject("ADODB.Recordset");
			var sql = "select * from group_ip";
			rs.open(sql, conn);
			while(!rs.EOF){
				json_arry.push({"group_id":rs.Fields("group_info").Value, "ip_info":rs.Fields("group_ip").Value});
				rs.moveNext();
			}
		}catch(e){
			document.write(e.description);
		}finally{
			this.db_closeAll(rs, conn);
		}
		return json_arry;
	}

	this.update_ip_info=function(index, vle){
		var conn = this.db_connect();
		var rs;
		try{
			rs = new ActiveXObject("ADODB.Recordset");
			var sql = "update group_ip set group_ip='" + vle + "' where group_info='" + index + "'";
			rs.open(sql, conn);
		}catch(e){
			document.write(e.description);
		}finally{
			this.db_closeAll(rs, conn);
		}
	}

	this.update_group_info=function(group_id, appid){
		var conn = this.db_connect();
		var rs;
		try{
			rs = new ActiveXObject("ADODB.Recordset");
			var sql = "update group_info set group_info='" + group_id + "' where appid='" + appid + "'";
			rs.open(sql, conn);
		}catch(e){
			document.write(e.description);
		}finally{
			this.db_closeAll(rs, conn);
		}
	}

	this.del_ip_info=function(index){
		var conn = this.db_connect();
		var rs;
		try{
			rs = new ActiveXObject("ADODB.Recordset");
			var sql = "delete from group_ip where group_id='" + index + "';";
			sql += "delete from group_info where group_info='" + index + "';";
			rs.open(sql, conn);
		}catch(e){
			document.write(e.description);
		}finally{
			this.db_closeAll(rs, conn);
		}
	}

	this.del_group_info=function(appid){
		var conn = this.db_connect();
		var rs;
		try{
			rs = new ActiveXObject("ADODB.Recordset");
			var sql = "delete from group_info where appid='" + appid + "';";
			rs.open(sql, conn);
		}catch(e){
			document.write(e.description);
		}finally{
			this.db_closeAll(rs, conn);
		}
	}

	this.add_ip_info=function(index, vle){
		var conn = this.db_connect();
		var rs;
		try{
			rs = new ActiveXObject("ADODB.Recordset");
			var sql = "insert into group_ip values ('" + index + "', '" + vle + "');";
			rs.open(sql, conn);
		}catch(e){
			document.write(e.description);
		}finally{
			this.db_closeAll(rs, conn);
		}
	}

	this.add_group_info=function(group_id, appid){
		var conn = this.db_connect();
		var rs;
		try{
			rs = new ActiveXObject("ADODB.Recordset");
			var sql = "insert into group_info values ('" + appid + "', '" + group_id + "');";
			rs.open(sql, conn);
		}catch(e){
			document.write(e.description);
		}finally{
			this.db_closeAll(rs, conn);
		}
	}

	this.db_closeAll=function(rs, conn){
		/*if(rs != null){
			rs.close();
			rs = null;
		}*/
		if(conn != null){
			conn.close();
			conn = null;
		}
	}
}

function moveTo(group_id, appid, old_group) {
	if(appid === "" || group_id === "main_content")
		return;
	console.log("group_id : " + group_id);
	console.log("appid : " + appid);
	console.log("old_group : " + old_group);

	if(group_id !== "del_li") {
		var manage = new DBManager();
		manage.update_group_info(group_id, appid);
		delete manage;
		return;
	}

	var manage = new DBManager();
	manage.del_group_info(appid);
	delete manage;
	
	$("#" + group_id)[0].removeChild($("#sortableListsHintWrapper")[0]);
}