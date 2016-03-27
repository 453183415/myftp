/*
 * log.cpp
 *
 *  Created on: 2015年11月19日
 *      Author: zjl
 */
#include"log.h"
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
static long long count=0;
void *fun(void *args){
	std::vector<std::string> s(1);
	for(int i=0;i<100;i++){
		s.push_back("aaaaaaaaaaaaaaaaaaaa"
				"ssssssssssssssssssss"
				"ssssssssss"
				);
	}
	for(int i=0; i<1;i++){
		log::get_instance()->writ_log(1,s);
		pthread_mutex_lock(&mutex);
		count+=50;
		pthread_mutex_unlock(&mutex);

	}
	return NULL;
}


int main()
{
	pthread_t pid;
	log * _log=log::get_instance();
	_log->init_log("/workspace/log",1000);
	for(int i=0 ;i<2;i++){
		pthread_create(&pid,NULL,fun,NULL);
		pthread_join(pid,NULL);
}
	log::get_instance()->clear();
	return 0;
}




