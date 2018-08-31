#include "XTCPEpollServer.h"

XTCPEpollServer::XTCPEpollServer()
{
    
}

XTCPEpollServer::~XTCPEpollServer()
{
    
}

void XTCPEpollServer::VOnRun(XThread* pThread)
{
	XInfo("-------------------------------------------------------------------------------- XTCPServer:OnRun() Begin\n");

    _Epoll.Create(1);
    _Epoll.Ctl(_Socket, EPOLL_CTL_ADD, EPOLLIN, this); 
	while (pThread->IsRun())
	{
		OnRunBegin();

        int nRet = _Epoll.Wait(1);
		if (EPOLL_ERROR == nRet)
		{
			pThread->Exit();
			break;
		}
		else if (0 == nRet)
		{
			continue;
		}
       
        epoll_event* pEvents = _Epoll.GetEvents();
        for (int i = 0; i < nRet; ++i)
        {
            if (pEvents[i].events & EPOLLIN)
            {
               Accept();    
            }
        }
	}
    _Epoll.Destroy();

	XInfo("-------------------------------------------------------------------------------- XTCPServer:OnRun() End\n");
}




