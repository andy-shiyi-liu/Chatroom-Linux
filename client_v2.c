/*****client.c*****/
// 1. 多进程：父进程用于发送客户端输入的信息，子进程用于接受服务器端发送的信息

/*****System libraries*****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*****Define*****/
#define MAXLINE 80
#define SERV_ADDR "101.42.141.88"
#define SERV_PORT 8000
#define MAXNAME 20

/*****Declaration of functions*****/

/*****main*****/
int main(int argc, char *argv[])
{
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	int sockfd, n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERV_ADDR, &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	// read服务器发出的输入用户昵称的信息
	read(sockfd, buf, MAXLINE);
	printf("%s", buf);
	//输入用户昵称并发送
	fgets(buf, MAXLINE, stdin);
	while (strlen(buf) - 1 > 20)
	{
		printf("Your name is too long!(>20)\n");
		fgets(buf, MAXLINE, stdin);
	}
	write(sockfd, buf, MAXLINE);

	// fork
	n = fork();

	if (n == -1)
	{ // fork失败
		printf("Fork error!\n");
		exit(1);
	}
	else if (n == 0)
	{ //子进程：接收服务器端发送的信息
		while (1)
		{
			n = read(sockfd, buf, MAXLINE);
			if (n == 0)
			{
				printf("The server has been closed.\n");
				break;
			}
			puts(buf);
		}
	}
	else
	{ //父进程：发送用户输入的信息
		while (fgets(buf, MAXLINE, stdin) != NULL)
		{
			write(sockfd, buf, MAXLINE);
			memset(buf, 0, sizeof(buf));
		}

		close(sockfd);
	}
	return 0;
}

/*****Definition of functions*****/
