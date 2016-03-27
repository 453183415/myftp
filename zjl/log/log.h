/*
 * log.h
 *
 *  Created on: 2015年11月19日
 *      Author: zjl
 */

#ifndef LOG_H_
#define LOG_H_
#include<iostream>
#include<pthread.h>
#include<string.h>
#include<stdio.h>
#include<vector>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include"log_queue.h"
#include<new>

class log{
private:
	static  log * instance;
	pthread_mutex_t* mutex;
	std::string dir_name;
	std::string log_name;
	int file_fd;
	int today;
	log_queue<std::string> * queue_log;

public:

	static log * get_instance();

	static void *clear_log_pthread(void *args){
		log::get_instance()->async_write_log();
		return NULL;
	}

	bool init_log(const std::string &file_name, int max_queue =1000   );
	void writ_log(int level, const std::vector <std::string>format);
	void clear();

private:

	log();
	virtual ~log();

	 void *async_write_log(){
		 std::string w_log;
		 	while(queue_log->pop_queue(w_log)){

		 		pthread_mutex_lock(mutex);
		 		write(file_fd,w_log.c_str(),sizeof(w_log));
		 		pthread_mutex_unlock(mutex);

		 	}
		 	return NULL;
	}
};
log*log::get_instance(){
	if(instance==NULL){
		pthread_mutex_lock(mutex);
		if(instance==NULL)
			instance=new log();
		pthread_mutex_unlock(mutex);
	}
	return instance;

}
log * log::instance=0;
log::log(){
	file_fd=-1;
	queue_log=NULL;
	today=0;
	try{
		mutex = new pthread_mutex_t;
	}catch(std::bad_alloc &b){
		std::cout <<"error1"<<std::endl;
	}
	pthread_mutex_init(mutex,NULL);

}

log::~log(){
	if(file_fd > 0){
		close(file_fd);
	}

	pthread_mutex_destroy(mutex);

	if(mutex != NULL)
	{
		delete mutex;
	}
	delete queue_log;
}


bool log::init_log(const std::string &file_name,  int max_queue){
	if(max_queue>= 1){
		try{

			queue_log = new log_queue<std::string> (max_queue);
		}catch(std::bad_alloc &b){
			std::cout<<"error"<<std::endl;
		}
		pthread_t pid;
		pthread_create(&pid,NULL,clear_log_pthread,NULL);
	}

	time_t t=time(NULL);
	struct tm*sys_tm=localtime(&t);
	struct tm * my_time =sys_tm;
	my_time->tm_year+=1990;
	my_time->tm_mon+=1;
	today=my_time->tm_mday;
	dir_name=file_name;
	log_name=asctime(my_time);
	std::cout<<log_name<<std::endl;
	dir_name += "/";
	std::string buf1;
	buf1=dir_name;
	std::cout<<log_name.size()<<std::endl;
	for(int i=0,j=0;i<log_name.size();i++){
		if(log_name[i]==' '){
			if(j==2)
				break;
			buf1+='_';
			j++;
		}else{
			buf1+=log_name[i];
		}
	}
	buf1+=".log";
	std::cout<<dir_name<<std::endl;
	file_fd = open(buf1.c_str(),O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
	printf("%d\n",file_fd);
	if(file_fd <0){
		return false;
	}
	return true;
}


void log::writ_log(int level ,const std::vector<std::string> format){

	struct timeval now;
	bzero(&now,sizeof(now));
	gettimeofday(&now,NULL);

	time_t t= now.tv_sec;
	struct tm * sys_time = localtime(&t);
	struct tm *my_time = sys_time;
	std::string model;
	switch(level){
	case 0 : model="[debug]";break;
	case 1 : model="[info]";break;
	case 2 : model = "[error]"; break;
	default : model = "[unknow]"; break;
	}

	pthread_mutex_lock(mutex);
	if(today != my_time->tm_mday){

		close(file_fd);
		std::string new_day;
		my_time->tm_year+=1990;
		my_time->tm_mon+=1;
		new_day=asctime(my_time);
		dir_name += "/";
		for(int i=0,j=0;i<new_day.size();i++){
				if(log_name[i]==' '){
					if(j==2)
						break;
					dir_name+='_';
					j++;
				}else
				dir_name+=log_name[i];
			}
		today = my_time->tm_mday;
		file_fd = open(dir_name.c_str(),O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
	}
	pthread_mutex_unlock(mutex);

	std::string write_buf;

	pthread_mutex_lock(mutex);
	write_buf=log_name;

	write_buf+=model;
	for(int i=0;i<format.size();i++){

		write_buf+=format[i];
		write_buf+="  ";
	}
	write_buf+="\n";

	pthread_mutex_unlock(mutex);
#if(1)
	if(!queue_log->full()){
		queue_log->push_queue(write_buf);
	}else
#endif

		{

		pthread_mutex_lock(mutex);
		std::cout<<write_buf<<std::endl;
		write(file_fd,write_buf.c_str(),write_buf.size());
		pthread_mutex_unlock(mutex);

	}

}

void log::clear(){

	pthread_mutex_lock(mutex);
	close(file_fd);
	pthread_mutex_unlock(mutex);

}

#endif /* LOG_H_ */
