extern struct userinfo users[MAXCAPACITY];
char notes[MAXLINE];
void listuser(char *buf, const int uid, char *BrdMsg)
{
    memset(BrdMsg, 0, sizeof(char) * MAXLINE);
    sprintf(BrdMsg,"\033[34mnumber   name   id   notes\033[37m");
    write(users[uid].fd, BrdMsg, MAXLINE);
    int k = 0;
    for(int i = 0; i < MAXCAPACITY; i++)
    {
        memset(BrdMsg, 0, sizeof(char) * MAXLINE);
        if (users[i].id == 0)
            continue;
        k++;
        if (users[i].ban)
        {
            sprintf(notes, "banned");
        }
        else
        {
            memset(notes, 0, sizeof(char) * MAXLINE);
        }
        if (i == uid)
        {
            sprintf(BrdMsg, "\033[35m  %d      %s\t%d    %s\033[37m", k, users[i].name, users[i].id, notes);
        }
        else
        {
            sprintf(BrdMsg, "\033[34m  %d      %s\t%d    %s\033[37m", k, users[i].name, users[i].id, notes);
        }
        write(users[uid].fd, BrdMsg, MAXLINE);
    }
        // ref: call private_chat() to send message
}