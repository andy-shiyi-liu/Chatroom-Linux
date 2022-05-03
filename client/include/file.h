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

#define FILE_PATH_LEN 128
#define FILE_SIZE_LEN 16
#define FILE_NAME_LEN 128
#define FILE_BUF_LEN 4096

struct fileinfo
{
    int size;
    char name[FILE_NAME_LEN];
    char uploader[MAXNAME];
    int id;
};

void getname(char file_path[], char file_name[]);
void upfile(char *buf);
void downfile(char *buf);
int str2int(char *str);
