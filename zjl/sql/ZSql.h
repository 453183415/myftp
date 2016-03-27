/*
 * ZSql.h
 *
 *  Created on: 2015年10月23日
 *      Author: zjl
 */

#ifndef ZSQL_H_
#define ZSQL_H_
#include<iostream>
#include<mysql/mysql.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

class ZSql{
public:
	ZSql();
	virtual ~ZSql();
	public :
		static ZSql *get_instance();
		bool ZSql_init(std::string db_name,std::string user_name,std::string db_ip,std::string db_pwd);
	public:
		MYSQL* getMysql(){
			if(mysql==NULL)
			{
				regsit();
			}
			return mysql;
		}
	protected:
		 MYSQL *mysql;
	private:
		bool regsit();
	private:
		pthread_mutex_t *mutex;
		 std::string DbName;
		 std::string UserName;
		 std::string DbIp;
		 std::string DbPasswd;
		 static ZSql * sql;
};
ZSql *  ZSql::sql=0;
ZSql::ZSql(){
	mysql=new MYSQL;
	 if(mysql_init(mysql)==NULL)
	 {
		 exit(0);
	 }
	 try{
		 mutex= new pthread_mutex_t;
	 }catch(std::bad_alloc &d){
		 std::cout<<"new ZSql mutex error"<<std::endl;
	 }
	 pthread_mutex_init(mutex,NULL);
}

ZSql::~ZSql(){
	mysql_close(mysql);
	pthread_mutex_destroy(mutex);

		if(mutex != NULL)
		{
			delete mutex;
		}
		delete mysql;
}

bool ZSql::regsit(){
	std::cout<<DbName<<UserName<<std::endl;
			if(mysql_real_connect(mysql,DbIp.c_str(),UserName.c_str(),DbPasswd.c_str(),
			DbName.c_str(),0,NULL,0)){

			return true;
		}else{
		 return false;
		}

}

ZSql * ZSql::get_instance(){
	if(sql==NULL){
//		pthread_mutex_lock(mutex);
		if(sql == NULL){
			sql = new ZSql();
		}
//		pthread_mutex_unlock(mutex);
	}
	return sql;
}
bool ZSql::ZSql_init(std::string db_name,std::string user_name,std::string db_ip,std::string db_pwd){
	DbName=db_name;
	UserName=user_name;
	DbIp=db_ip;
	DbPasswd=db_pwd;
	if(regsit()){
		return true;
	}
	return false;
}

#endif /* ZSQL_H_ */
