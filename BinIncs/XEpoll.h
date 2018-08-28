#ifndef __XEPOLL_H__
#define __XEPOLL_H__

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define EPOLL_ERROR (-1)

class XEpoll
{
public:
    XEpoll();
    ~XEpoll();	

    int Create(int maxClient);
    int Destroy();
    int Ctl(SOCKET socket, int op, uint32_t event, void* ptr);
    int Wait(int t);
    epoll_event* GetEvents();

private:
    int _Epoll;
    int _MaxClient;
    epoll_event* _pEvents;
};

#endif
