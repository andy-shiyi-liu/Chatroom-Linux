#define MAXLINE 80

struct userinfo
{
	int fd;
	char name[MAXLINE];
	char ip[15];
	int id;
	int ban;
};
