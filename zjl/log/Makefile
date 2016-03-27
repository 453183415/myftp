#it is log.cpp makefile
prog: log.o
	g++ log.o -lpthread -o prog
log.o:log.cpp log.h log_queue.h 
	g++ -c log.cpp

