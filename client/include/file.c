#include "file.h"

void upfile(char *buf)
{
    /* TODO:
    1. 输入文件路径
    2. 由文件路径提取文件名称
    3. 打开文件
    4. 分段发送文件，显示发送进度
    */

    char temp[3];
    char file_path[FILE_PATH_LEN], file_name[FILE_NAME_LEN] = {0};

    //询问用户想要上传的文件路径
    printf("Enter file path: ");
    fgets(file_path, FILE_PATH_LEN, stdin);
    file_path[strlen(file_path) - 1] = 0; //删除fgets末尾的回车符

    //打开指定文件，并判断文件是否存在
    int fp;                         //文件指针
    fp = open(file_path, O_RDONLY); //以只读模式打开该文件
    if (fp == -1)                   //打开失败
    {
        printf("File '%s' does not exist!\n", file_path);
        return;
    }

    write(sockfd, buf, sizeof(buf)); //成功打开文件后，将/upfile命令发送给服务器，以供其识别

    //接收服务器端文件库是否满的信息
    read(sockfd, buf, MAXLINE);
    if (strcmp(buf, "FULL") == 0) //服务器发来已满的信息
    {
        printf("Server file repository is full. Aborting upload!\n");
        return;
    }

    //发送文件头（大小和名称）
    struct fileinfo file; 
    memset(file.name, 0, sizeof(file.name));
    getname(file_path, file.name);      //获取file的纯名称
    file.size = lseek(fp, 0, SEEK_END); //计算文件大小
    lseek(fp, 0, SEEK_SET);             //将fp偏移回文件开头

    //将文件大小和名称以字符串形式存于filehead，并发送给服务器
    char filehead[FILE_SIZE_LEN + FILE_NAME_LEN];
    bzero(filehead, sizeof(filehead));
    sprintf(filehead, "%d", file.size);
    strcpy(filehead + FILE_SIZE_LEN, file.name);

    // read(sockfd, temp, sizeof(temp)); //阻塞，等待服务器“可以发送”命令
    write(sockfd, filehead, sizeof(filehead));

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
            printf("Upload file '%s' succeed!\n", file.name);
            break;
        }
        sent_len += read_len;

        read(sockfd, temp, sizeof(temp)); //阻塞，等待服务器“可以发送”命令
        write(sockfd, filebuf, read_len); //发送给server

        printf("Uploading %.2f%%\n", (float)sent_len / file.size * 100); //显示上传文件的百分比
    }
    close(fp);
}

void downfile(char *buf)
{

    char file_name[FILE_NAME_LEN] = {0};

    write(sockfd, buf, MAXLINE); //将/downfile指令发送给服务器端

    printf("Enter file name to download:");
    fgets(file_name, FILE_NAME_LEN, stdin);
    file_name[strlen(file_name) - 1] = 0;    //删除fgets末尾的回车符
    write(sockfd, file_name, FILE_NAME_LEN); //将文件名发送给服务器端

    //接收服务器返回的文件状态（是否存在）信息，若发生错误则打印错误信息，中断下载
    read(sockfd, buf, MAXLINE);
    if (strcmp(buf, "ERROR: NO SUCH FILE") == 0)
    {
        printf("File '%s' does not exist!\n", file_name);
        return;
    }
    if (strcmp(buf, "ERROR: FILE DELETED") == 0)
    {
        printf("File '%s' has been deleted!\n", file_name);
        return;
    }

    //从服务器接收文件大小，以供接收时显示进度和判断接收完成
    int file_size;
    char filesize_str[FILE_SIZE_LEN] = {0};

    write(sockfd, "//", 3); //告知服务器可以发送
    read(sockfd, filesize_str, FILE_SIZE_LEN);
    file_size = str2int(filesize_str);
    printf("Receiving file '%s', size=%d\n", file_name, file_size);

    //创建文件
    char file_path[FILE_PATH_LEN];
    int fp;

    sprintf(file_path, "./files/%s", file_name);
    fp = open(file_path, O_RDWR | O_CREAT | O_TRUNC, 0666);

    //接收文件
    int recv_len = 0; //已接收的文件大小
    int read_len;     //一次read的文件大小
    char filebuf[FILE_BUF_LEN];

    write(sockfd, "//", 3); //告知服务器可以发送
    while (1)
    {
        read_len = read(sockfd, filebuf, FILE_BUF_LEN);

        recv_len += read_len;
        printf("Receiving %.2f%%\n", (float)recv_len / file_size * 100);

        if (recv_len == file_size) //传输完成
        {
            printf("Receive file '%s' succeed!\n", file_name);
            close(fp);
            break;
        }

        write(fp, filebuf, read_len);
        write(sockfd, "//", 3); //告知服务器可以发送
    }
}

void getname(char file_path[], char file_name[])
{
    // TODO: file_path = pure_path/file_name
    int i, slash_pos;

    //找到最后一个斜杠的位置，赋值给slash_pos
    for (i = 0; file_path[i] != 0; i++)
        if (file_path[i] == '/')
            slash_pos = i;
    //将最后一个slash之后的内容（文件名称）拷贝到file_name
    strncpy(file_name, file_path + slash_pos + 1, strlen(file_path) - slash_pos - 1);
}

int str2int(char *str)
{
    int i;
    int num = 0;
    for (i = 0; str[i] != 0; i++)
        num = 10 * num + str[i] - '0';
    return num;
}