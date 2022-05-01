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
	pthread_t tid;
};
#endif