#include "manager.h"

extern struct userinfo users[MAXCAPACITY];
extern int usernum;

void kick(char *buf, const int uid, char *BrdMsg)
{
    /* TODO:
    1. ask passwd
    2. if passed right, ask nickname
    3. kick
    */

    // ask and receive manager passwd
    strcpy(buf, "Please input manager password.");
    write(users[uid].fd, buf, MAXLINE);
    memset(buf, 0, sizeof(char) * MAXLINE);
    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // check that password is correct
    if (strcmp(buf, MANAGER_PASSWD) != 0)
    {
        strcpy(buf, "\033[31mWRONG password!\033[37m");
        write(users[uid].fd, buf, MAXLINE);
        memset(buf, 0, sizeof(char) * MAXLINE);
        return;
    }

    // ask for user information to be kicked
    strcpy(buf, "Please input the name of the user to be kicked.");
    write(users[uid].fd, buf, MAXLINE);
    memset(buf, 0, sizeof(char) * MAXLINE);
    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // kick user
    for (int i = 0; i < MAXCAPACITY; i++)
    {
        if (strcmp(buf, users[i].name) == 0)
        {
            if (i == uid)
            {
                strcpy(buf, "\033[31mYou are kicking yourself. Use /quit instead.\033[37m");
                write(users[uid].fd, buf, MAXLINE);
                return;
            }

            int threadCancelState = pthread_cancel(users[i].tid);
            if (threadCancelState != 0)
            {
                strcpy(buf, "\033[31mThead cancel wrong\033[37m");
                write(users[uid].fd, buf, MAXLINE);
                return;
            }

            strcpy(buf, "\033[31mYou have been kicked. Bye.\033[37m");
            write(users[i].fd, buf, MAXLINE);

            if (close(users[i].fd) != 0)
            {
                strcpy(buf, "\033[31mSocket close wrong\033[37m");
                write(users[uid].fd, buf, MAXLINE);
                return;
            }

            sprintf(buf, "\033[31mKicked user '%s', uid is %d\033[37m", users[i].name, users[i].id);
            write(users[uid].fd, buf, MAXLINE);
            printf("\033[31mLOG: user %s kicked user %s\n\033[37m", users[uid].name, users[i].name);
            sprintf(BrdMsg, "\033[31mUser %s kicked user %s\033[37m", users[uid].name, users[i].name);
            send2all(BrdMsg, users[uid].id);
            memset(buf, 0, sizeof(char) * MAXLINE);
            memset(BrdMsg, 0, sizeof(char) * MAXLINE);

            // set user info to init state
            users[i].fd = 0;
            users[i].name[0] = 0;
            users[i].ip[0] = 0;
            users[i].ban = false;
            users[i].id = 0;
            users[i].tid = 0;
            usernum--;

            return;
        }
    }

    // user not found
    sprintf(buf, "\033[31mUser with name '%s' NOT found.\033[37m", buf);
    write(users[uid].fd, buf, MAXLINE);
}

void ban(char *buf, const int uid, char *BrdMsg)
{
    /* TODO:
    1. ask passwd
    2. if passed right, ask nickname
    3. ban
    */

    // ask and receive manager passwd
    strcpy(buf, "Please input manager password.");
    write(users[uid].fd, buf, MAXLINE);
    memset(buf, 0, sizeof(char) * MAXLINE);
    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // check that password is correct
    if (strcmp(buf, MANAGER_PASSWD) != 0)
    {
        strcpy(buf, "\033[31mWRONG password!\033[37m");
        write(users[uid].fd, buf, MAXLINE);
        memset(buf, 0, sizeof(char) * MAXLINE);
        return;
    }

    // ask for user information to be banned
    strcpy(buf, "Please input the name of the user to be banned.");
    write(users[uid].fd, buf, MAXLINE);
    memset(buf, 0, sizeof(char) * MAXLINE);

    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // ban user
    for (int i = 0; i < MAXCAPACITY; i++)
    {
        if (strcmp(buf, users[i].name) == 0)
        {
            users[i].ban = true;
            sprintf(buf, "\033[31mBanned user '%s', uid is %d\033[37m", users[i].name, users[i].id);
            write(users[uid].fd, buf, MAXLINE);
            printf("\033[31mLOG: user %s banned user %s\n\033[37m", users[uid].name, users[i].name);
            sprintf(BrdMsg, "\033[31mUser %s banned user %s\033[37m", users[uid].name, users[i].name);
            send2all(BrdMsg, users[uid].id);
            memset(BrdMsg, 0, sizeof(char) * MAXLINE);
            return;
        }
    }

    // user not found
    sprintf(buf, "\033[31mUser with name '%s' NOT found.\033[37m", buf);
    write(users[uid].fd, buf, MAXLINE);
    memset(buf, 0, sizeof(char) * MAXLINE);
}

void unban(char *buf, const int uid, char *BrdMsg)
{
    /* TODO:
    1. ask passwd
    2. if passed right, ask nickname
    3. unban
    */

    // ask and receive manager passwd
    strcpy(buf, "Please input manager password.");
    write(users[uid].fd, buf, MAXLINE);
    memset(buf, 0, sizeof(char) * MAXLINE);

    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // check that password is correct
    if (strcmp(buf, MANAGER_PASSWD) != 0)
    {
        strcpy(buf, "\033[31mWRONG password!\033[37m");
        write(users[uid].fd, buf, MAXLINE);
        memset(buf, 0, sizeof(char) * MAXLINE);
        return;
    }

    // ask for user information to be unbanned
    strcpy(buf, "Please input the name of the user to be unbanned.");
    write(users[uid].fd, buf, MAXLINE);
    memset(buf, 0, sizeof(char) * MAXLINE);
    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // unban user
    for (int i = 0; i < MAXCAPACITY; i++)
    {
        if (strcmp(buf, users[i].name) == 0)
        {
            users[i].ban = false;
            sprintf(buf, "\033[31mUnbanned user '%s', uid is %d\033[37m", users[i].name, users[i].id);
            write(users[uid].fd, buf, MAXLINE);
            printf("\033[31mLOG: user %s unbanned user %s\n\033[37m", users[uid].name, users[i].name);
            sprintf(BrdMsg, "\033[31mUser %s unbanned user %s\033[37m", users[uid].name, users[i].name);
            send2all(BrdMsg, users[uid].id);
            memset(buf, 0, sizeof(char) * MAXLINE);
            memset(BrdMsg, 0, sizeof(char) * MAXLINE);
            return;
        }
    }

    // user not found
    sprintf(buf, "\033[31mUser with name '%s' NOT found.\033[37m", buf);
    write(users[uid].fd, buf, MAXLINE);
}