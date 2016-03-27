/*
 * write.h
 *
 *  Created on: 2015年10月22日
 *      Author: zjl
 */

#ifndef WRITE_H_
#define WRITE_H_

#include<sys/types.h>
#include<iostream>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<read_queue.h>
#include"../log/log.h"
class writ{
	private:
		pthread_mutex_t *mutex;
		static writ *instance;
		read_queue<int>*queue_w;
	public:
		static writ *get_instance();
		static void *handle(void *args);
		void insert_w_msg(const int &fd);//向队列插入socket描述符
		void writ_init();
	private:
		virtual ~writ();
		writ();
		void *handle_queue();//处理队列;

};
writ* writ::instance=0;
writ::writ(){
	mutex=new pthread_mutex_t;
	queue_w=0;
	pthread_mutex_init(mutex,NULL);
}

writ::~writ(){
	pthread_mutex_destroy(mutex);
	if(mutex!=NULL)
		delete mutex;
	queue_w->~read_queue();
	if(queue_w!=NULL)
		delete queue_w;
}

void writ::writ_init(){
	queue_w=new read_queue<std::map<int,std::string>>;
	pthread_t pid;
	pthread_create(&pid,NULL,handle,NULL);
}

static writ* writ::get_instance(){
	if(instance == NULL)
//		pthread_mutex_lock(mutex);
		if(instance == NULL)
			instance= new writ;
//	pthread_mutex_unlock(mutex);
	return instance;
}

static void writ::*handle(void args){
	writ::get_instance()->handle_queue();
	return NULL;
}

void writ::insert_w_msg(const int &confd){
		queue_w->push(confd);
}

void * writ::handle_queue(){
		int s;
		 	 int fd,n;
		 	 char buf[1024];
			 while(queue_w->pop(s)){
				 bzero(&buf,1024);
				 pthread_mutex_lock(mutex);
				 read(s,buf,1024);
				 fd=open(buf,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
				 if(fd<0){
					 std::vector<std::string>s1;
					 s1.push_back("open file error");
					 s1.push_back(buf);
					 log::get_instance()->writ_log(2,s1);//				 添加 log error
				 }

				 sleep(1);
				 while((n=read(s,buf,1024))!=0){
					 write(fd,buf,sizeof(buf));
					 bzero(buf,1024);
				 }
				 close(fd);
				 close(s);
				 pthread_mutex_unlock(mutex);
			 }

	return NULL;
}

#endif /* WRITE_H_ */
