#include "XEpollServer.h"

int XEpollServer::VInit()
{
    _Epoll.Create(102400);  
    return 0;
}

int XEpollServer::VDone()
{
    _Epoll.Destroy();
    return 0;
}

int XEpollServer::VClientJoin(std::shared_ptr<XClient> pClient)
{
    _Epoll.Ctl(pClient->GetSocket(), EPOLL_CTL_ADD, EPOLLIN, pClient.get());
    return 0;
}

int XEpollServer::VDoNetEvent()
{
    for (auto client : _AllClients)
    {
        if (client.second->GetFlush())
        {
            _Epoll.Ctl(client.second->GetSocket(), EPOLL_CTL_MOD, EPOLLIN | EPOLLOUT, client.second.get());
        }
        else
        {
            _Epoll.Ctl(client.second->GetSocket(), EPOLL_CTL_MOD, EPOLLIN, client.second.get());
        }
    }

    int nRet = _Epoll.Wait(1); 
    if (EPOLL_ERROR == nRet)
    {
        XError("Wait Error!\n");
        return -1;
    }
    else if (0 == nRet)
    {
        return 0;
    }
    
    epoll_event* pEvents = _Epoll.GetEvents();
    for (int i = 0; i < nRet; ++i)
    {
        XClient* pClient = (XClient*)pEvents[i].data.ptr;
        if (pClient == nullptr)
        {
            XWarn("Epoll ptr is nullptr.\n");
            continue;
        }

        if (pEvents[i].events & EPOLLIN)
        {
            int nRet = pClient->RecvMsg();
            if (nRet != 0)
            {
                if (_pGlobalEventObj)
                    _pGlobalEventObj->OnClientLeave(pClient->GetSharedPtr());
                
                auto iter = _AllClients.find(pClient->GetSocket());
                if (iter != _AllClients.end())
                    _AllClients.erase(iter);

                continue;
            }
        }
        
        if (pEvents[i].events & EPOLLOUT)
        {
            int nRet = pClient->SendMsg();
            if (nRet < 0)
            {
                if (_pGlobalEventObj)
                    _pGlobalEventObj->OnClientLeave(pClient->GetSharedPtr());

                auto iter = _AllClients.find(pClient->GetSocket());
                if (iter != _AllClients.end())
                    _AllClients.erase(iter);
            }
        }
    }
    

   return nRet;
}
