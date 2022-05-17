/*****server.c*****/
// Ver:2.1
// 1. 多线程（提前创建线程，使用户登录不会阻塞）
// 2. 包含用户昵称系统
// 3. 收到用户发送的信息后，广播给其他用户；其中一个用户退出时，将该信息广播给其他用户
// 4. 用户id号唯一，每有一个新用户登录，userid++，且不随用户退出而--

/***待改进问题***/
//（已解决）1. 创建线程可以提早（在连接成功时就pthread_create），以免其中一个用户还在登录时，其他用户无法登录（即阻塞在read用户昵称处）
// 2. 私聊功能
//（已解决）3. 进场welcome
// 4. 文件传输
// 5. 命令系统，如：退出命令(-quit)、查看用户列表(-list)、查看命令列表(-help)、私聊(-private uid)
// 6. 聊天记录保存和查询功能
// 7. 用户输入时若有消息进入，会直接冲掉正在输入的内容（用户端可能要使用多线程，在有信息到来时，若检测到输入缓冲区不为零，则将缓冲区内容拷贝到临时buffer中，待显示完消息后再复制回输入缓冲区）

/*****System libraries*****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*****Declaration of functions*****/
void *service_thread(void *useri);
void delete_tail_enter(char *string);
void send2all(char *msg, int current_id);

/*****Define*****/
#define VERSION "2.1"
#define MAXLINE 80	   // buf的容量
#define SERV_PORT 8000 //服务器端口号
#define MAXCAPACITY 30 //聊天室最大用户数量
#define MAXNAME 20	   //最长用户昵称
#define true 1
#define false 0

/*****Custom libraries*****/
#include "./include/server_v2.h"
#include "./include/manager.c"
#include "./include/at.c"
#include "./include/file.c"
#include "./include/listuser.c"
#include "./include/private.c"
#include "./include/sendone.c"

/*****Extern Variables*****/
struct userinfo users[MAXCAPACITY]; //结构体变量users储存已连接用户的昵称和ip地址
int usernum = 0;					//聊天室中现有人数
int userid = 1;						//用户id，只递增，用户退出时不递减
int connfd;
struct sockaddr_in cliaddr;
socklen_t cliaddr_len;

struct fileinfo files[MAXFILENUM]; // files变量
int fileid;						   //文件id计数器，只增不减，原理同userid

/*****main*****/
int main()
{
	//识别./files/目录下现有的文件，加入文件列表
	file_init();

	struct sockaddr_in servaddr;
	int listenfd;
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	int i, n;

	// socket
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	// reuse socket.
	int opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	//配置服务器地址
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	// bind and listen
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(listenfd, 20);

	printf("Version:%s\nServer started.\nAccepting connections...\n", VERSION);

	//循环接收客户端连接
	while (1)
	{
		//等待接受客户端连接accept
		cliaddr_len = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
		//有连接请求时，判断聊天室是否满员，若是，则在服务器和正等待连接的客户端打印错误信息，否则创建新线程
		if (usernum >= MAXCAPACITY)
		{
			printf("The chatroom is full!\n");
			strcpy(buf, "Sorry, the chatroom is full!");
			write(connfd, buf, MAXLINE);
		}
		else
		{
			pthread_t tid;
			int temp_uid;

			usernum++;

			for (i = 0; i < MAXCAPACITY && users[i].id != 0; i++)
				; //查找空闲的用户信息存储位置
			temp_uid = i;
			//创建新线程tid
			pthread_create(&tid, 0, service_thread, &i); //把当前user的所有信息的序号i的地址传入service_thread函数
			users[temp_uid].tid = tid;					 // 记录用户对应的线程信息
		}
	}
}

/*****Definition of functions*****/
// service_thread:收取该线程对应用户端的信息，调用send2all函数发送至其他所有用户，并在服务器端显示
void *service_thread(void *useri)
{
	int upfile_1st = 1, downfile_1st = 1;

	const int i = *(int *)useri;
	int n;
	char buf[MAXLINE], BrdMsg[MAXLINE]; // BrdMsg存储需要广播的信息

	//用户信息配置
	users[i].fd = connfd;	// users.fd
	users[i].id = userid++; // users.id 从1开始编号users.id,id=0表示空
	users[i].ban = false;
	users[i].name[0] = 0; // init user name

	// 为了实现踢出功能，在这里设置线程属性为接受其他线程取消此线程，并在接受到取消请求时立即退出
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);

	//提示用户设定昵称，并存储
	strcpy(buf, "Designate Your User Name: ");
	write(users[i].fd, buf, MAXLINE);
	memset(buf, 0, sizeof(char) * MAXLINE);
	read(users[i].fd, buf, MAXLINE); //接收用户传来的昵称名信息
	delete_tail_enter(buf);

	int repeated_name = true;
	while (repeated_name == true)
	{
		repeated_name = false;
		for (int j = 0; j < MAXCAPACITY; j++)
		{
			if (strcmp(buf, users[j].name) == 0) // 如果重名 提示重新指定名称
			{
				repeated_name = true;
				memset(buf, 0, sizeof(char) * MAXLINE);
				strcpy(buf, "Name Repeated, redesignate your name: ");
				write(users[i].fd, buf, MAXLINE);
				memset(buf, 0, sizeof(char) * MAXLINE);
				read(users[i].fd, buf, MAXLINE); //接收用户传来的昵称名信息
				delete_tail_enter(buf);
				break;
			}
		}
	}

	strcpy(users[i].name, buf);

	strcpy(users[i].ip, inet_ntoa(cliaddr.sin_addr));									// users.ip
	printf("LOGIN:\tUser:%s\tIP:%s\tID:%d\n", users[i].name, users[i].ip, users[i].id); //服务器端打印登录用户的信息

	//广播当前用户进入聊天室的信息
	memset(BrdMsg, 0, sizeof(char) * MAXLINE);
	sprintf(BrdMsg, "%s has entered the chatroom, uid=%d", users[i].name, users[i].id);
	send2all(BrdMsg, users[i].id); //将有名字的版本发给其他用户
	memset(BrdMsg, 0, sizeof(char) * MAXLINE);
	sprintf(BrdMsg, "You have entered the chatroom, uid=%d", users[i].id);
	write(users[i].fd, BrdMsg, MAXLINE); //将“你”称呼的版本发给当前用户

	int toall = 1;
	int getname = 0;
	int temp = 0;
	memset(BrdMsg, 0, sizeof(char) * MAXLINE);
	memset(buf, 0, sizeof(char) * MAXLINE);

	while (1)
	{
		n = read(users[i].fd, buf, MAXLINE);
		if (n == 0)
		{
			sprintf(BrdMsg, "User %s has quit, uid=%d", users[i].name, users[i].id); //将要发送的信息输入到BrdMsg字符数组

			printf("LOGOUT:\tUser:%s\tIP:%s\tID:%d\n", users[i].name, users[i].ip, users[i].id); //打印至服务器端屏幕

			send2all(BrdMsg, users[i].id); //发送到每个客户
			memset(buf, 0, sizeof(char) * MAXLINE);
			memset(BrdMsg, 0, sizeof(char) * MAXLINE);

			close(users[i].fd);

			// set user info to init state
			users[i].fd = 0;
			users[i].name[0] = 0;
			users[i].ip[0] = 0;
			users[i].ban = false;
			users[i].id = 0;
			users[i].tid = 0;
			usernum--;

			pthread_exit(NULL); //退出此线程
		}

		delete_tail_enter(buf);
		if (users[i].ban == true)
		{
			// allow another to unban himself
			if (strcmp(buf, "/kick") == 0) // kick s.b. out
			{
				kick(buf, i, BrdMsg);
			}
			else if (strcmp(buf, "/ban") == 0) // ban s.b.
			{
				ban(buf, i, BrdMsg);
			}
			else if (strcmp(buf, "/unban") == 0) // unban s.b.
			{
				unban(buf, i, BrdMsg);
			}
			else // if not manager commands
			{
				// todo: prompt user that he is banned
				strcpy(buf, "\033[31mYou are banned now!\033[37m");
				write(users[i].fd, buf, MAXLINE);
				memset(buf, 0, sizeof(char) * MAXLINE);
				memset(BrdMsg, 0, sizeof(char) * MAXLINE);
				continue;
			}
		}
		else if (buf[0] == '@') //@ sb to send private highlight message.
		{
			at(buf, i, BrdMsg); // qzj修改了这里的函数调用
								/*printf("receive message: ");
								printf("%s(uid=%d):%s\n", users[i].name, users[i].id, buf);
								sprintf(BrdMsg, "%s:\t%s", users[i].name, buf);
								send2all(BrdMsg, users[i].id);*/
		}
		else if (strcmp(buf, "/upfile") == 0) // upload file to server
		{
			if (upfile_1st == 1)
				write(users[i].fd, "CONTINUE_UP", MAXLINE); //无意义，只是为了冲掉client端“读”线程的read
			upfile(i);
			upfile_1st = 0;
		}
		else if (strcmp(buf, "/downfile") == 0) // download file to local
		{
			if (downfile_1st == 1)
				write(users[i].fd, "CONTINUE_DOWN", MAXLINE); //无意义，只是为了冲掉client端“读”线程的read
			downfile(i);
			downfile_1st = 0;
		}
		else if (strcmp(buf, "/listfile") == 0) //向客户端发送文件列表
		{
			listfile(i);
		}
		else if (strcmp(buf, "/kick") == 0) // kick s.b. out
		{
			kick(buf, i, BrdMsg);
		}
		else if (strcmp(buf, "/ban") == 0) // ban s.b.
		{
			ban(buf, i, BrdMsg);
		}
		else if (strcmp(buf, "/unban") == 0) // unban s.b.
		{
			unban(buf, i, BrdMsg);
		}
		else if (strcmp(buf, "/toall") == 0) // quit private chat
		{
			char msg[150] = {};
			strcpy(msg, "您已退出私聊模式。");
			sendone(msg, users[i].fd, i);
			toall = 1;
		}
		else if (strcmp(buf, "/private") == 0) // enter private chat
		{
			toall = 0;
			getname = 1;
			char msg[150] = {};
			strcpy(msg, "您已选择向特定用户发送信息模式，请输入您想私戳的用户");
			sendone(msg, users[i].fd, i);
		}
		else if (toall == 0) // continue to the private chat
		{
			if (getname == 1)
			{
				for (temp = 0; temp < usernum; temp++)
				{
					if (strcmp(users[temp].name, buf) == 0)
					{ //查找私戳对象
						getname = 0;
						break;
					}
				}
				if (temp == usernum)
				{ //用户查找失败，重新查找
					char *str = "您输入的名称不存在，请重新输入！";
					sendone(str, users[i].fd, i);
					getname = 1;
				}
				else
				{
					sprintf(buf, "%s向您发起私戳。", users[i].name); //向对方发出私戳邀请
					sendone(buf, users[temp].fd, i);
				}
			}
			else
			{
				sendone(buf, users[temp].fd, i);
			}
			/*else
			{
				if(strcmp(users[temp].name,buf)==0){     //用户查找成功，进行私戳
					strcpy(buf,"向您发起私戳。");  //向对方发出私戳邀请
				}
				sendone(buf,users[temp].fd,i);
				//sendone("\n",users[temp].fd,i);
			}*/
		}
		else if (strcmp(buf, "/listuser") == 0)
		{
			listuser(buf, i, BrdMsg);
		}
		else
		{
			printf("receive message: ");
			printf("%s(uid=%d):%s\n", users[i].name, users[i].id, buf);
			sprintf(BrdMsg, "%s:\t%s", users[i].name, buf); // qzj修改了此处
			send2all(BrdMsg, users[i].id);
		}
		memset(buf, 0, sizeof(char) * MAXLINE);
		memset(BrdMsg, 0, sizeof(char) * MAXLINE);
	}
}

// delete_tail_enter删除读取buf时读入的字符串末尾回车
void delete_tail_enter(char *string)
{
	if (string[strlen(string) - 1] == '\n')
		string[strlen(string) - 1] = 0;
}

// send2all:
//传入参数：所发信息字符串msg，所有用户信息的结构体数组users，当前用户id（为0时全部发送）
void send2all(char *msg, int current_id)
{
	int i;
	for (i = 0; i < MAXCAPACITY; i++)
	{
		if (users[i].id != 0 && users[i].id != current_id)
		{
			write(users[i].fd, msg, MAXLINE);
		}
	}
}
