#include "at.h"

int k = 0;

char message[MAXLINE];
const char *ALL = "all";
char newbuf[MAXLINE];
extern struct userinfo users[MAXCAPACITY];

void fine_name(char *buf, int uid)
{
    char destname[MAXLINE];
    for (int i = 0; i < MAXLINE; i++)
    {
        destname[i] = '\0';
    }
    for (int i = 1; buf[i] != ' ' && buf[i] != '@'; i++)
    {
        destname[i - 1] = buf[i];
        k = i;
        if (buf[i] == '\0')
            break;
    }

    while (buf[k] != '\0' && buf[k] != '@')
    {
        k++;
    }

    if (buf[k] == '@')
        fine_name(buf + k, uid);

    if (strcmp(destname, ALL) == 0)
    {
        for (int i = 0; i < MAXCAPACITY; i++)
        {
            users[i].at = 1;
        }
    }
    else
    {
        for (int i = 0; i < MAXCAPACITY; i++)
        {
            if (strcmp(destname, users[i].name) == 0)
            {
                users[i].at = 1;
            }
        }
    }
}

void at(char *buf, const int uid, char *BrdMsg)
{

    printf("receive message: ");
    printf("%s(uid=%d):%s\n", users[uid].name, users[uid].id, buf);
    fine_name(buf, uid);

    for (int i = 0; i < MAXCAPACITY; i++)
    {
        if (i == uid || users[i].id == 0)
            continue;
        else if (users[i].at)
        {
            sprintf(BrdMsg, "\033[35m%s:\t%s\033[37m", users[uid].name, buf);
        }
        else
        {
            sprintf(BrdMsg, "%s:\t%s", users[uid].name, buf);
        }

        write(users[i].fd, BrdMsg, MAXLINE);
    }
    for (int i = 0; i < MAXCAPACITY; i++)
    {
        users[i].at = 0;
    }
}
