#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "server_v2.h"

// #define MAXLINE 80     // buf的容量
// #define SERV_PORT 8000 //服务器端口号
// #define MAXCAPACITY 30 //聊天室最大用户数量
// #define true 1
// #define false 0
#define MANAGER_PASSWD "123"

void kick(char *buf, const int uid, char *BrdMsg);

void ban(char *buf, const int uid, char *BrdMsg);

void unban(char *buf, const int uid, char *BrdMsg);