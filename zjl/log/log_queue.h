/*
 * bolck_queue.h
 *
 *  Created on: 2015年11月19日
 *      Author: zjl
 */

#ifndef LOG_QUEUE_H_
#define LOG_QUEUE_H_
#include<iostream>
#include<stdio.h>
#include<pthread.h>
#include<sys/time.h>
#include<stdlib.h>
template <class T>
class log_queue{
private:
	pthread_mutex_t *Mutex;
	pthread_cond_t *Cond;
	T *array;
	int max_size;
	int front;
	int tail;
	int size;

public:

	log_queue(int _ssize = 1000):max_size(_ssize){
		if(max_size <0){
			exit(0);
		}
		try{

			array = new T[max_size];
		}catch(std::bad_alloc &g){
			std::cout<<"log_queue errorr"<<std::endl;
		}
		size =0;
		front = -1;
		tail = -1;
		Mutex = new pthread_mutex_t;
		Cond = new pthread_cond_t;
		pthread_mutex_init(Mutex,NULL);
		pthread_cond_init(Cond,NULL);
	}

	void clear(){
		pthread_mutex_lock(Mutex);
		size = 0;
		front = -1;
		tail = -1;
		pthread_mutex_unlock(Mutex);
	}

	~log_queue(){
		pthread_mutex_lock(Mutex);
		if(array != NULL){
			delete array;
		}
		pthread_mutex_unlock(Mutex);
		pthread_mutex_destroy(Mutex);
		pthread_cond_destroy(Cond);
		delete Mutex;
		delete Cond;
	}

	bool full ()const{

		pthread_mutex_lock(Mutex);

		if(size>= max_size){
			pthread_mutex_unlock(Mutex);
			return true;
		}

		pthread_mutex_unlock(Mutex);
		return false;
	}

	bool empty()const{
		pthread_mutex_lock(Mutex);

		if(max_size == 0){
			pthread_mutex_unlock(Mutex);
			return true;
		}

		pthread_mutex_unlock(Mutex);

		return false;
	}

	bool m_front(T & value){

		pthread_mutex_lock(Mutex);
		if(0 == max_size){
			pthread_mutex_unlock(Mutex);
			return false;;
		}

		value = array[front];
		pthread_mutex_unlock(Mutex);

		return true;
	}

	bool m_tail(T & value){
		pthread_mutex_lock(Mutex);
		if(0 == max_size){
			pthread_mutex_unlock(Mutex);
			return false;;
		}

		value = array[front];
		pthread_mutex_unlock(Mutex);
		return true;
	}

	int get_size(){
		int temp;
		pthread_mutex_lock(Mutex);
		temp= size;
		pthread_mutex_unlock(Mutex);
		return temp;
	}

	int get_max_size(){
		int temp;
		pthread_mutex_lock(Mutex);
		temp= max_size;
		pthread_mutex_unlock(Mutex);
		return temp;
	}

	bool pop_queue(T & item){

		pthread_mutex_lock(Mutex);
		if(size<=0){
			pthread_cond_broadcast(Cond);
			pthread_mutex_unlock(Mutex);
			return false;
		}
		front= (front+1)%max_size;
		item = array[front];
		size--;
		pthread_cond_broadcast(Cond);
		pthread_mutex_unlock(Mutex);
		return true;
	}

	bool push_queue(T & item){
		pthread_mutex_lock(Mutex);
		if(size >= max_size){
			pthread_cond_broadcast(Cond);
			pthread_mutex_unlock(Mutex);
			return false;
		}

		tail = (tail+1)%max_size;
		array[tail]= item;
		size++;
		pthread_cond_broadcast(Cond);
		pthread_mutex_unlock(Mutex);
		return true;
	}
};



#endif /* LOG_QUEUE_H_ */
