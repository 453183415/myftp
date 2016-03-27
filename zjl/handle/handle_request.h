/*
 * handle_request.h
 *
 *  Created on: 2015年10月23日
 *      Author: zjl
 */

#ifndef HANDLE_REQUEST_H_
#define HANDLE_REQUEST_H_
#include<sys/socket.h>
#include<dirent.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<iostream>
#include<signal.h>
#include"read.h"
int sock_con(const struct sockaddr_in &client){
	int sockfd;
	struct sockaddr_in clientaddr;
	bzero(&clientaddr,sizeof(clientaddr));
	clientaddr.sin_family= AF_INET;
	clientaddr.sin_port = htons(8001);
	clientaddr.sin_addr.s_addr=client.sin_addr.s_addr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
//			signal(SIGCHLD,handleSig);
	int ret =connect(sockfd,(struct sockaddr *)&clientaddr,sizeof(clientaddr));

	return sockfd;
}
void sock_acc(const struct sockaddr_in &client){
	int fd,sockfd,opt=1;

	struct sockaddr_in serveraddr,clientaddr;
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family= AF_INET;
	serveraddr.sin_port = htons(8000);
	serveraddr.sin_addr.s_addr= client.sin_addr.s_addr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	bind(sockfd,(sockaddr *)&serveraddr,sizeof(serveraddr));
	listen(sockfd,1024);
	socklen_t clientlen;
	clientlen= sizeof(clientaddr);
	while(1){
		fd=accept(sockfd,(struct sockaddr *)&clientaddr,&clientlen);
		writ::get_instance()->insert_w_msg(fd);
	}
}

void handle_dir(const struct sockaddr_in &client, DIR *DP,const char *path){
	struct dirent *file_dir;
	struct stat file;
	char filename[256];
		bzero(&filename,256);
	while((file_dir=readdir(DP))!= NULL){
				if(strcmp(file_dir->d_name,".")==0||strcmp(file_dir->d_name,"..")==0){
					continue;
				}
	//			printf("%s\n",file->d_name);
				sprintf(filename,"%s%s",path,file_dir->d_name);

				if(lstat(filename,&file)<0){
					continue;
				}
				if(!S_ISDIR(file.st_mode)){
					std::string file_path;
					file_path=filename;
					int fd=sock_con(client);
					reader::get_instance()->insert_msg(fd,file_path);
				}
				if(S_ISDIR(file.st_mode)){
					handle_dir(client,DP,strcat(filename,"/"));
				}
			}

}
void hand_file(const struct sockaddr_in &client,const std::string &path){
	int fd;
	fd=sock_con(client);
	reader::get_instance()->insert_msg(fd,path);
}
bool handle_request(const int & fd,const std::string &file_path){
	DIR *DP;
	struct sockaddr_in client;
	unsigned int length=sizeof(client);
	getpeername(fd,(struct sockaddr *)&client,&length);
	int file_fd;
	if((DP=opendir(file_path.c_str())==NULL )){
		if((file_fd=access(file_path.c_str(),F_OK))<0)
		return false;
	}
	if(DP!=NULL){
		handle_dir(client,DP,file_path.c_str());
		return true;
	}
	hand_file(client,file_path);
	return true;
}

void hanfle_put(const int &confd){
	struct sockaddr_in client;
	unsigned int length=sizeof(client);
	getpeername(confd,(struct sockaddr*)&client,&length);
	sock_acc(client);

}


#endif /* HANDLE_REQUEST_H_ */
