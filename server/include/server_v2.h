#ifndef _SERVER_V2_H_
#define _SERVER_V2_H_
#include <pthread.h>

#define MAXLINE 80
// typedef unsigned long pthread_t;

struct userinfo  
{
	int fd;
	char name[MAXLINE];
	char ip[15];
	int id;
	int ban;
	int at;   //qzj增加了at属性
	pthread_t tid;
};
#endif