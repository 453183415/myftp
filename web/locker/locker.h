/*
 * locker.h
 *
 *  Created on: 2015年11月3日
 *      Author: zjl
 */

#ifndef LOCKER_H_
#define LOCKER_H_

#include<exception>
#include<pthread.h>
#include<semaphore.h>
#include<iostream>
class sem{
	public:
		sem(){
			if(sem_init(&m_sem,0,0) != 0){
				throw std::exception();
			}
		}
		virtual~sem(){
			sem_destroy(&m_sem);
		}
		bool wait(){
			return sem_wait(&m_sem) == 0;
		}
		bool post(){
			return sem_post(&m_sem) == 0;
		}
	private:
		sem_t m_sem;
};

class locker{
	private:
		pthread_mutex_t mutex;
	public:
		locker(){
			if(pthread_mutex_init (&mutex,NULL) != 0){
				throw std::exception();
			}
		}
		virtual ~locker(){
			pthread_mutex_destroy(&mutex);
		}
		bool lock(){
			return pthread_mutex_lock(&mutex) == 0;
		}
		bool unlock(){
			return pthread_mutex_unlock(&mutex) == 0;
		}
};

class cond{
	private:
		pthread_mutex_t mutex;
		pthread_cond_t cond;
	public:
		cond(){
			if( pthread_mutex_init(&mutex,NULL) != 0){
				throw std::exception();
			}

		if(pthread_cond_init(&cond,NULL) != 0){
			pthread_mutex_destroy(&mutex);
			throw std::exception();
		}
	}
		virtual ~cond(){
			pthread_mutex_destroy(&mutex);
			pthread_cond_destroy(&cond);
		}
		bool wait(){
			int ret =0;
			pthread_mutex_lock(&mutex);
			ret = pthread_cond_wait(&cond,&mutex);
			pthread_mutex_unlock(&mutex);
			return ret == 0;
		}
		bool sifnal(){
			return pthread_cond_signal(&cond) == 0;
		}
};
#endif /* LOCKER_H_ */
