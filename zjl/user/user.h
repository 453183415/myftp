/*
 * user.h
 *
 *  Created on: 2015年10月27日
 *      Author: zjl
 */

#ifndef USER_USER_H_
#define USER_USER_H_
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<iostream>
#include"../sql/ZSql.h"
#include"../log/log.h"
#include"readAndwrite/read.h"
#include"readAndwrite/write.h"
#include"../handle/handle_request.h"
void open_file(const char * path,const int fd){
	DIR *DP;
	struct dirent *file_dir;
	struct stat file;
		char filename[256];
	while((file_dir=readdir(DP))!= NULL){
					if(strcmp(file_dir->d_name,".")==0||strcmp(file_dir->d_name,"..")==0){
						continue;
					}
					write(fd,file_dir->d_name,sizeof(file_dir->d_name));
	}

}

void *get_pthread(void *args){
	std::map<int ,std::string > path=(std::map<int,std::string>*)args;
	reader::get_instance()->insert_msg(path.begin()->first,path.begin()->second);
	return NULL;
}

void *put_pthread(void *args){
    int  client= (int *)args;
	writ::get_instance()->insert_w_msg(client);
	return NULL;
}

void user(const int &fd){
	char buf[1024];
	int n;
	pthread_t pid;
	int status;
	std::string query;
	char username[25];
	char passwd[25];
	char  commond[10];
	char path[256];
	struct sockaddr_in client;
//	getpeername(fd,(struct sockaddr*)&client,NULL);
	while(1){
		bzero(&buf,1024);
		n=read(fd,buf,1024);
		sscanf(buf,"%s %s",username,passwd);
		query="select username,passwd from user where" +"username="+username+",passwd="+passwd;
		mysql_real_query(ZSql::get_instance()->getMysql(),query.c_str(),query.size());
		MYSQL *result = mysql_store_result(ZSql::get_instance()->getMysql());
		if(result != NULL){
			write(fd,"success",8);
			break;
		}
		write(fd,"error",5);
	}
	while(1){
		bzero(&buf,1024);
		read(fd,buf,1024);
		sprintf(buf,"%s %s",commond,path);
		if(commond == "cd"||commond == "CD"){
			if(opendir(path)!=NULL){
				write(fd,path,sizeof(path));
			}else{
				write(fd,"error path",10);
			}
		}
		if(commond=="ls"||commond=="LS"){
			if(opendir(path)!=NULL){
							open_file(path,fd);
						}else{
							write(fd,"error path",10);
						}

		}
		if(commond=="GET"|| commond=="get"){
			if(opendir(path)!=NULL){
							write(fd,"successed",10);
							std::map<int ,std::string>a(fd,commond);
							pthread_create(&pid,NULL,get_pthread,&a);
							pthread_join(pid,NULL);
						}else{
							write(fd,"error path",10);
						}

		}
		if(commond=="PUT"||commond=="put"){
			if(opendir(path)!=NULL){
										write(fd,"successed",10);
										int confd=fd;
										pthread_create(&pid,NULL,put_pthread,&confd);
										pthread_join(pid,NULL);
									}else{
										write(fd,"error path",10);
									}

		}
		if(commond=="BYE"||commond=="bye"){
			write(fd,"bye",3);
			close(fd);
			exit(0);
		}else{
			write(fd,"error commond",13);
		}
	}
}




#endif /* USER_USER_H_ */
