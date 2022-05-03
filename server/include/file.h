#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// added head files
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#define FILE_PATH_LEN 128
#define FILE_SIZE_LEN 16
#define FILE_NAME_LEN 128
#define FILE_BUF_LEN 4096
#define MAXFILENUM 5

struct fileinfo
{
    int size;
    char name[FILE_NAME_LEN];
    char uploader[MAXNAME];
    int id; //文件id从1开始计数，id=0表示位置为空
};

void getname(char file_path[], char file_name[]);
int str2int(char *str);
void upfile(const int uid);
void downfile(const int uid);
void file_init(void);
void listfile(int i);
