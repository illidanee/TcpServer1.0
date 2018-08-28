#ifndef __XTCPEPOLLSERVER_H__
#define __XTCPEPOLLSERVER_H__

#include "XTCPServer.h"
#include "XEpoll.h"

//监听Server类
class XTCPEpollServer : public XTCPServer
{
public:
	virtual void VOnRun(XThread* pThread);

private:
    XEpoll _Epoll;
};


#endif
