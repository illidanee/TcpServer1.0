#ifndef __XEPOLLSERVER_H__
#define __XEPOLLSERVER_H__

#include "XServer.h"
#include "XEpoll.h"

class XEpollServer : public XServer
{
public:
    virtual int VInit();
    virtual int VDone();

    virtual int VClientJoin(std::shared_ptr<XClient> pClient);
	virtual int VDoNetEvent();

private:
    XEpoll _Epoll;
};

#endif
