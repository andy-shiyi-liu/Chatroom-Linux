/*****client.c*****/
//多线程

/*****System libraries*****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

/*****Define*****/
#define MAXLINE 80
#define SERV_ADDR "101.42.141.88"
#define SERV_PORT 8000
#define MAXNAME 20

/*****Extern Variables*****/
int sockfd;
int download, upload;
char buf_read[MAXLINE], buf_write[MAXLINE];

/*****User libs*****/
#include "./include/at.c"
#include "./include/manager.c"
#include "./include/file.c"
#include "./include/listuser.c"
#include "./include/private.c"

/*****Declaration of functions*****/
void *service_thread(void *);

/*****main*****/
int main(int argc, char *argv[])
{
	struct sockaddr_in servaddr;
	int n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERV_ADDR, &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	// read服务器发出的输入用户昵称的信息
	read(sockfd, buf_read, MAXLINE);
	printf("%s", buf_read);
	//输入用户昵称并发送
	fgets(buf_read, MAXLINE, stdin);
	while (strlen(buf_read) - 1 > 20)
	{
		printf("Your name is too long!(>20)\n");
		fgets(buf_read, MAXLINE, stdin);
	}
	write(sockfd, buf_read, MAXLINE);

	pthread_t tid;
	pthread_create(&tid, 0, service_thread, NULL); //创建线程，用于发送信息，主线程用于接收信息

	while (1)
	{
		//如果客户端处于上传文件或下载状态，则将对应标志置1，客户端暂停接收并显示信息
		if ((download == 1) || (upload == 1))
			continue;
		n = read(sockfd, buf_read, MAXLINE);
		if ((download == 1) || (upload == 1))
			continue;

		if (n == 0)
		{
			printf("The server has been closed.\n");
			break;
		}
		puts(buf_read);
	}
	return 0;
}

/*****Definition of functions*****/
void *service_thread(void *p)
{
	while (fgets(buf_write, MAXLINE, stdin) != NULL)
	{
		buf_write[strlen(buf_write) - 1] = 0; // fgets会读入回车符，因此需要手动将末尾回车符设置为EOF(0)
		if (strcmp(buf_write, "/downfile") == 0)
		{
			download = 1;
			downfile(buf_write);
			download = 0;
		}
		else if (strcmp(buf_write, "/upfile") == 0)
		{
			upload = 1;
			upfile(buf_write);
			upload = 0;
		}
		else if (strcmp(buf_write, "/private") == 0)
		{
			private_chat(buf_write);
		}
		else if (strcmp(buf_write, "/quit") == 0)
		{
			close(sockfd);
			exit(0);
		}
		else
		{
			write(sockfd, buf_write, MAXLINE);
			memset(buf_write, 0, sizeof(buf_write));
		}
	}
	pthread_exit(NULL); //退出此线程
	close(sockfd);
}