/*
 * zjl.cpp
 *
 *  Created on: 2015年11月10日
 *      Author: zjl
 */

#include"connect/connection.h"
#include"user/user.h"

int main(int argc, char**argv)
{

	int fd;
	pid_t pid;
	 Connect con;
	con.listend(8000);
	while(1){
		fd=con.accepted();
		if((pid=fork())==0){
			close(con.get_listenfd());
			user(fd);
		}
	}
}



