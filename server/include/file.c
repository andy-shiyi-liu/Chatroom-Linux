#include "file.h"

extern struct userinfo users[MAXCAPACITY];
extern struct fileinfo files[MAXFILENUM];
extern int fileid; //文件id计数器，只增不减，原理同userid

void upfile(const int uid)
{
    /* TODO:
    1. 接收fileinfo结构体的文件头（大小和文件名）
    2. 分段接收文件内容，存于./files目录下，并保留原有文件名
    3. 将文件名和上传者添加至索引，以供查看？
    */

    // 初始化文件头变量
    char filehead[FILE_SIZE_LEN + FILE_NAME_LEN];
    bzero(filehead, sizeof(filehead));

    //设置当前文件的信息
    int i;
    for (i = 0; files[i].id != 0 && i < MAXFILENUM; i++)
        ; //查找id为0，即空闲的位置

    if (i == MAXFILENUM) //若文件库已满，告知客户端中断传输
    {
        write(users[uid].fd, "FULL", MAXLINE);
        return;
    }
    else //若文件库未满，也告知客户端继续传输
    {
        write(users[uid].fd, "OK", MAXLINE); //不会在客户端打印
    }

    // write(users[uid].fd, "//", 3);                   //告知客户端可以发送
    read(users[uid].fd, filehead, sizeof(filehead)); //读入文件头

    files[i].size = str2int(filehead);               //文件大小
    strcpy(files[i].name, filehead + FILE_SIZE_LEN); //文件名
    strcpy(files[i].uploader, users[uid].name);      //文件上传者

    printf("Receiving file '%s', size=%d\n", files[i].name, files[i].size); //服务器端显示

    //服务器端创建待接收新文件
    char filepath[FILE_PATH_LEN];
    bzero(filepath, FILE_PATH_LEN);
    sprintf(filepath, "./files/%s", files[i].name);

    int fp = open(filepath, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fp <= 0)
    {
        printf("Fail to open file!\n");
        return;
    }

    //服务器端接收文件
    int recv_len = 0; //已接收的文件大小
    int read_len;     //一次read操作的文件大小
    char filebuf[FILE_BUF_LEN];
    write(users[uid].fd, "//", 3); //告知客户端可以发送
    while (1)
    {
        read_len = read(users[uid].fd, filebuf, FILE_BUF_LEN);

        recv_len += read_len;
        printf("Receiving %.2f%%\n", (float)recv_len / files[i].size * 100);

        if (recv_len == files[i].size) //传输完成
        {
            printf("Receive file '%s' succeed!\n", files[i].name);
            close(fp);
            files[i].id = ++fileid; //上传完成后，将file id计数器+1，并设置文件id
            break;
        }
        write(fp, filebuf, read_len);

        write(users[uid].fd, "//", 3); //告知客户端可以发送
    }
}

void downfile(const int uid)
{
    char temp[3];
    char buf[MAXLINE];
    char file_name[FILE_NAME_LEN] = {0};
    char file_path[FILE_NAME_LEN] = "./files/";
    int i;

    //读入用户想下载的文件名，判断在服务器文件库中是否有该文件（在files结构数组和files目录下双重查找）
    read(users[uid].fd, file_name, FILE_NAME_LEN);
    //在files结构数组内查找
    for (i = 0; i < MAXFILENUM; i++)
    {
        if (files[i].id != 0 && strcmp(file_name, files[i].name) == 0) //查找成功，确定要下载的文件id=i
            break;
    }
    if (i == MAXFILENUM) //文件列表中无该文件
    {
        write(users[uid].fd, "ERROR: NO SUCH FILE", MAXLINE);
        return;
    }
    //在files目录下查找，若未找到则文件已被删除，返回错误信息并将对应的files[i].id置0
    strcat(file_path, file_name);
    int fp = open(file_path, O_RDONLY);
    if (fp == -1)
    {
        write(users[uid].fd, "ERROR: FILE DELETED", MAXLINE);
        files[i].id = 0;
        return;
    }
    write(users[uid].fd, "OK", MAXLINE); //结束客户端的等待状态（客户端不会显示）

    //将文件大小以字符串形式存于filesize_str，并发送给客户端
    char filesize_str[FILE_SIZE_LEN];
    bzero(filesize_str, FILE_SIZE_LEN);
    sprintf(filesize_str, "%d", files[i].size);

    read(users[uid].fd, temp, sizeof(temp)); //阻塞，等待客户端“可以发送”命令
    write(users[uid].fd, filesize_str, sizeof(filesize_str));

    //发送文件
    int sent_len = 0;           //已发送的大小
    int read_len;               //一次读取文件的大小
    char filebuf[FILE_BUF_LEN]; //文件缓存器

    while (1)
    {
        memset(filebuf, 0, sizeof(filebuf));

        read_len = read(fp, filebuf, FILE_BUF_LEN); //读取文件数据
        if (read_len <= 0)                          //文件已读完
        {
            printf("Client download file '%s' succeed!\n", file_name);
            break;
        }
        sent_len += read_len;

        read(users[uid].fd, temp, sizeof(temp)); //阻塞，等待客户端“可以发送”命令
        write(users[uid].fd, filebuf, read_len); //发送给客户端
    }
    close(fp);
}

void file_init(void) //读取目录下的文件，并将信息填入files结构体数组
{
    const char dirname[20] = "./files/";
    DIR *dp = opendir(dirname);
    struct dirent *filename;
    int i = 0;
    int fd;
    char filepath[FILE_PATH_LEN];

    while ((filename = readdir(dp)) != NULL)
    {
        if (strcmp(filename->d_name, ".") == 0 || strcmp(filename->d_name, "..") == 0)
            continue;
        strcpy(files[i].name, filename->d_name); //拷贝文件名称
        strcpy(files[i].uploader, "SERVER");     //客户端启动时已有文件上传者为SERVER

        //获取文件大小
        sprintf(filepath, "%s%s", dirname, files[i].name);
        fd = open(filepath, O_RDONLY);
        files[i].size = lseek(fd, 0, SEEK_END); //计算文件大小
        close(fd);

        files[i].id = i + 1; //文件id
        i++;
    }

    closedir(dp);
}

void listfile(int uid)
{
    int i;
    char buf[MAXLINE];
    write(users[uid].fd, "/**************FILE LIST**************/", MAXLINE);
    write(users[uid].fd, "Name      \tUploader  \tSize      \t", MAXLINE);
    for (i = 0; i < MAXFILENUM; i++)
    {
        if (files[i].id != 0)
        {
            sprintf(buf, "%-10s\t%-10s\t%-10d", files[i].name, files[i].uploader, files[i].size);
            write(users[uid].fd, buf, MAXLINE);
        }
    }
}

int str2int(char *str)
{
    int i;
    int num = 0;
    for (i = 0; str[i] != 0; i++)
        num = 10 * num + str[i] - '0';
    return num;
}
