/*
 * read_queue.h
 *
 *  Created on: 2015年10月22日
 *      Author: zjl
 */

#ifndef READ_QUEUE_H_
#define READ_QUEUE_H_
#include<stdlib.h>
#include<iostream>
#include<stdio.h>
#include<unistd.h>
#include<queue>
#include<pthread.h>
template <class T>
class read_queue{
private:
	pthread_mutex_t *mutex;
	std::deque<T >queue;
public:
	virtual ~ read_queue(){
		pthread_mutex_destroy(mutex);
		if(mutex!=NULL){
			delete mutex;
		}
		clear();
	}
	read_queue(){
		mutex = new pthread_mutex_t;
		pthread_mutex_init(mutex,NULL);
	}
	void push(const T & item){
		pthread_mutex_lock(mutex);
		queue.push_back(item);
		pthread_mutex_unlock(mutex);
	}
	bool pop(T & item){
		pthread_mutex_lock(mutex);
		if(queue.empty()){
			pthread_mutex_unlock(mutex);
			return false;
		}
		item=queue.front();
		queue.pop_front();
		pthread_mutex_unlock(mutex);

	}
	int get_size(){
		int num;
		pthread_mutex_lock(mutex);
		num=queue.size();
		pthread_mutex_unlock(mutex);
		return num;
	}
	void clear(){
		queue.~deque();
	}

};

#endif /* READ_QUEUE_H_ */
