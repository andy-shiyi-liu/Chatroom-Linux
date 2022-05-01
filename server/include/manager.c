#include "manager.h"

extern struct userinfo users[MAXCAPACITY];

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
    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // check that password is correct
    if (strcmp(buf, MANAGER_PASSWD) != 0)
    {
        strcpy(buf, "WRONG password!");
        write(users[uid].fd, buf, MAXLINE);
        return;
    }

    // ask for user information to be kicked
    strcpy(buf, "Please input the name of the user to be kicked.");
    write(users[uid].fd, buf, MAXLINE);
    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // kick user
    for (int i = 0; i < MAXCAPACITY; i++)
    {
        if (strcmp(buf, users[i].name) == 0)
        {
            int threadCancelState = pthread_cancel(users[i].tid);
            if (threadCancelState != 0)
            {
                strcpy(buf, "Thead cancel wrong");
                write(users[uid].fd, buf, MAXLINE);
                return;
            }

            strcpy(buf, "You have been kicked. Bye.");
            write(users[i].fd, buf, MAXLINE);

            if (close(users[i].fd) != 0)
            {
                strcpy(buf, "Socket close wrong");
                write(users[uid].fd, buf, MAXLINE);
                return;
            }

            // set user info to init state
            users[i].fd = 0;
            users[i].name[0] = 0;
            users[i].ip[0] = 0;
            users[i].ban = false;
            users[i].id = 0;
            users[i].tid = 0;

            sprintf(buf, "Kicked user '%s', uid is %d", users[i].name, users[i].id);
            write(users[uid].fd, buf, MAXLINE);
            printf("LOG: user %s kicked user %s", users[uid].name, users[i].name);
            sprintf(BrdMsg, "User %s kicked user %s", users[uid].name, users[i].name);
            send2all(BrdMsg, users[uid].id);
            return;
        }
    }

    // user not found
    sprintf(buf, "User with name '%s' NOT found.", buf);
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
    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // check that password is correct
    if (strcmp(buf, MANAGER_PASSWD) != 0)
    {
        strcpy(buf, "WRONG password!");
        write(users[uid].fd, buf, MAXLINE);
        return;
    }

    // ask for user information to be banned
    strcpy(buf, "Please input the name of the user to be banned.");
    write(users[uid].fd, buf, MAXLINE);
    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // ban user
    for (int i = 0; i < MAXCAPACITY; i++)
    {
        if (strcmp(buf, users[i].name) == 0)
        {
            users[i].ban = true;
            sprintf(buf, "Banned user '%s', uid is %d", users[i].name, users[i].id);
            write(users[uid].fd, buf, MAXLINE);
            printf("LOG: user %s banned user %s", users[uid].name, users[i].name);
            sprintf(BrdMsg, "User %s banned user %s", users[uid].name, users[i].name);
            send2all(BrdMsg, users[uid].id);
            return;
        }
    }

    // user not found
    sprintf(buf, "User with name '%s' NOT found.", buf);
    write(users[uid].fd, buf, MAXLINE);
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
    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // check that password is correct
    if (strcmp(buf, MANAGER_PASSWD) != 0)
    {
        strcpy(buf, "WRONG password!");
        write(users[uid].fd, buf, MAXLINE);
        return;
    }

    // ask for user information to be unbanned
    strcpy(buf, "Please input the name of the user to be unbanned.");
    write(users[uid].fd, buf, MAXLINE);
    read(users[uid].fd, buf, MAXLINE);
    delete_tail_enter(buf);

    // unban user
    for (int i = 0; i < MAXCAPACITY; i++)
    {
        if (strcmp(buf, users[i].name) == 0)
        {
            users[i].ban = false;
            sprintf(buf, "Unbanned user '%s', uid is %d", users[i].name, users[i].id);
            write(users[uid].fd, buf, MAXLINE);
            printf("LOG: user %s unbanned user %s", users[uid].name, users[i].name);
            sprintf(BrdMsg, "User %s unbanned user %s", users[uid].name, users[i].name);
            send2all(BrdMsg, users[uid].id);
            return;
        }
    }

    // user not found
    sprintf(buf, "User with name '%s' NOT found.", buf);
    write(users[uid].fd, buf, MAXLINE);
}