/*
 * connection.h
 *
 *  Created on: 2015年11月2日
 *      Author: zjl
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<arpa/inet.h>
#include<iostream>
#include<string.h>
#define LISTENQ 1024
namespace st{
	int open_listend(int port){
		int listenfd,optval=1;
			struct sockaddr_in serveraddr;

			if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0){
				return -1;
			}
			if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&optval,sizeof(int))<0){
				return -1;
			}
			bzero((char*)&serveraddr,sizeof(serveraddr));
			serveraddr.sin_family=AF_INET;
			serveraddr.sin_port=htons((unsigned short)port);
			serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
			if(bind(listenfd,(sockaddr *)& serveraddr,sizeof(serveraddr))<0)
				return -1;
			if(listen(listenfd,LISTENQ)<0){
				return-1;
			}
			return listenfd;
	}
	int Accept(int s, struct sockaddr *addr,socklen_t *addrlen){
		int rc;
			if((rc=accept(s,addr,addrlen))<0)
				printf("accept error");
			return rc;
	}
}
class Connect{
	public:
		Connect();
		void listend(int port);
		int accepted();
		int get_listenfd();
		void closed();
		int get_listenfd();
		virtual ~Connect();
	private:
		int confd;
		int listenfd;

};

int Connect::get_listenfd(){
	return listenfd;
}
int Connect::get_listenfd(){
	return listenfd;
}

void Connect::closed(){
	close(confd);
}
Connect::~Connect(){
	closed();
}
void Connect::listend(int port){
	this->listenfd=st::open_listend(port);
}
Connect::Connect():confd(-1),listenfd(-1){


}
int connect::accepted(){
	int clientlen;
		struct sockaddr_in clientaddr;
		clientlen=sizeof(clientaddr);
		confd=st::Accept(listenfd,(sockaddr *)&clientaddr,reinterpret_cast<socklen_t*>(&clientlen));
	return confd;
}

#endif /* CONNECTION_H_ */
