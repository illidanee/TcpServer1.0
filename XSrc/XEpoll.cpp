#include "XEpoll.h"

XEpoll::XEpoll()
{
	_Epoll = EPOLL_ERROR;
    _MaxClient = 0;
    _pEvents = nullptr;
}

XEpoll::~XEpoll()
{
    //Destroy();
}

int XEpoll::Create(int maxClient)
{
    assert(EPOLL_ERROR == _Epoll);

    _Epoll = epoll_create(maxClient);
    if (EPOLL_ERROR == _Epoll)
        return -1;

    _MaxClient = maxClient;

    _pEvents = new epoll_event[maxClient];
    
    return 0;
}

int XEpoll::Destroy()
{
    assert(EPOLL_ERROR != _Epoll);

    int nRet = close(_Epoll);
    if (EPOLL_ERROR == nRet)
        return -1;
    _Epoll = EPOLL_ERROR;

    _MaxClient = 0;

    delete[] _pEvents;
    _pEvents = nullptr;

    return 0;
}
    
int XEpoll::Ctl(SOCKET socket, int op, uint32_t ev, void* ptr)
{
    assert(EPOLL_ERROR != _Epoll);
    
    epoll_event event;
    event.events = ev;
    event.data.ptr = ptr;
    if (EPOLL_ERROR == epoll_ctl(_Epoll, op, socket, &event))
    {
        return -1;        
    }
    return 0;
}

int XEpoll::Wait(int t)
{
    assert(EPOLL_ERROR != _Epoll);

    return epoll_wait(_Epoll, _pEvents, _MaxClient, t);
}

epoll_event* XEpoll::GetEvents()
{
    return _pEvents;
}

