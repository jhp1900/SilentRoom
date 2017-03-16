/*
* 数据库操作
*
*/
function DBManager(){
	this.db_connect=function(){
		var conn = new ActiveXObject("ADODB.Connection");
		conn.Open("Provider=SQLOLEDB.1;Data Source=10.18.3.62,1433;User ID=sa;Password=718811107;Initial Catalog=RBS");
		return conn;
	}

	this.reload_stu=function(){
		var conn = this.db_connect();
		var rs;
		var json_arry = [];
		try{
			rs = new ActiveXObject("ADODB.Recordset");
			var sql = "select * from student join group_info on(student.appid = group_info.appid)";
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
				json_arry.push({"group_id":rs.Fields("group_id").Value, "ip_info":rs.Fields("ip_info").Value});
				rs.moveNext();
			}
		}catch(e){
			document.write(e.description);
		}finally{
			this.db_closeAll(rs, conn);
		}
		return json_arry;
	}

	this.db_closeAll=function(rs, conn){
		if(rs != null){
			rs.close();
			rs = null;
		}
		if(conn != null){
			conn.close();
			conn = null;
		}
	}
}