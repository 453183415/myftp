/*
 * read.h
 *
 *  Created on: 2015年10月22日
 *      Author: zjl
 */

#ifndef READ_H_
#define READ_H_
#include<sys/types.h>
#include<fcntl.h>
#include<dirent.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<pthread.h>
#include<iostream>
#include"read_queue.h"
#include"../log/log.h"
class reader{
	private:
		static reader *reade;
		pthread_mutex_t *mutex;
		read_queue<std::map<int,std::string >>*rd_queue;
	public:
		static reader*get_instance();
		static void *handle(void *args);
		void insert_msg(const int &fd, const std::string &path);//向队列中插入要处理的文件和socket描述符
		void *reader_init();
	private:
		virtual ~reader();
		reader();
		void *handle_queue();//处理队列;
};
reader*reader::reade=0;

reader::reader(){
	mutex=new pthread_mutex_t;
	pthread_mutex_init(mutex,NULL);
	rd_queue=NULL;
}

reader::~reader(){
	pthread_mutex_destroy(mutex);
	if(mutex==NULL){
		delete mutex;
	}
	rd_queue->~read_queue();
}

reader * reader::get_instance(){
	if(reader==NULL){
		if(reader == NULL){
			reade = new reader();
		}
	}
	return reader;
}

void reader::insert_msg(const int &fd, const std::string &path){
	std::map<int,std::string>s(fd,path);
	rd_queue->push(s);
}

void* reader::reader_init(){
	rd_queue = new read_queue<std::map<int,std::string>;
	pthread_t pid;
	pthread_create(&pid,NULL,handle,NULL);
}

 void *reader::handle(void * args){
	reader::get_instance()->handle_queue();
	 return NULL;
}

 void *reader::handle_queue(){
	 std::map<int,std::string>s;
	 int fd,n;
	 char buf[1024];
		 while(rd_queue->pop(s)){
			 bzero(&buf,1024);
			 pthread_mutex_lock(mutex);
			 fd=open(s.begin()->second.c_str(),O_RDONLY);
			 if(fd<0){
				 std::vector<std::string>s1;
				 s1.push_back("open file error");
				 s1.push_back(s.begin()->second);
				 log::get_instance()->writ_log(2,s1);//				 添加 log error
			 }
			 write(s.begin()->first,s.begin()->second.c_str(),sizeof(s.begin()->second));
			 sleep(1);
			 while((n=read(fd,buf,1024))!=0){
				 write(s.begin()->first,buf,sizeof(buf));
				 bzero(buf,1024);
			 }
			 close(fd);
			 close(s.begin()->first);
			 pthread_mutex_unlock(mutex);
		 }
		 return NULL;
 }

 #endif /* READ_H_ */
