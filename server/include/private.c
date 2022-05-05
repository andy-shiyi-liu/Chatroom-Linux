void private_chat(char *buf, char name_i, int fd_temp)
{ //实现私聊功能
    char msg[150] = {};
    time_t nowtime;
    time(&nowtime);
    sprintf(msg, "\033[46m%s%s:%s(这是一条私聊消息)\033[0m", ctime(&nowtime), name_i, buf);
    /*char buf2[1024];                                        //记录私戳信息到log.txt
    FILE *logs = fopen("log.txt","a+");
    if(logs==NULL){
        printf("open file error\n");
    }
    else{
        sprintf(buf2,"%s\n",msg);
        fputs(buf2,logs);
        fclose(logs);
    }*/
    write(fd_temp, msg, MAXLINE);
}