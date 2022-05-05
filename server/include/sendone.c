void sendone(char* msg, int fd_i,int i){      //发送消息给指定用户
    printf("send to %d\n", fd_i);
	/*char buf3[1024];
	FILE *logs = fopen("log.txt","a+");   //记录消息到log.txt
    if(logs==NULL){
        printf("open file error\n");
    }
    else{
        sprintf(buf3,"%s\n",msg);
		fputs(buf3,logs);
        sprintf(buf3,"the message is '%s'", msg);
        printf("%s\n", buf3);
        fclose(logs);
    }*/
	write(fd_i, msg, MAXLINE);
}