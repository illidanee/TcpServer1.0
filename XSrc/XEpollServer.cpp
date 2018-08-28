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
    _Epoll.Ctl(pClient->GetSocket(), EPOLL_CTL_ADD, EPOLLIN, &pClient);
    return 0;
}

int XEpollServer::VDoNetEvent()
{
    for (auto client : _AllClients)
    {
        if (client.second->GetFlush())
        {
            _Epoll.Ctl(client.second->GetSocket(), EPOLL_CTL_MOD, EPOLLIN | EPOLLOUT, &client.second);
        }
        else
        {
            _Epoll.Ctl(client.second->GetSocket(), EPOLL_CTL_MOD, EPOLLIN, &client.second);
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
        std::shared_ptr<XClient> pClient = *(std::shared_ptr<XClient>*)pEvents[i].data.ptr;
        if (pEvents[i].events & EPOLLIN)
        {
            int nRet = pClient->RecvMsg();
            if (nRet != 0)
            {
                if (_pGlobalEventObj)
                    _pGlobalEventObj->OnClientLeave(pClient);
                
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
                    _pGlobalEventObj->OnClientLeave(pClient);

                auto iter = _AllClients.find(pClient->GetSocket());
                if (iter != _AllClients.end())
                    _AllClients.erase(iter);
            }
        }
    }
    

   return 0;
}
