#include "XSelectServer.h"

int XSelectServer::DoNetEvent()
{
	//设置_FdRead
	if (_ClientChange)
	{
		_FdRead.Zero();

		_MaxSocketID = 0;
		for (std::map<SOCKET, std::shared_ptr<XClient>>::iterator iter = _AllClients.begin(); iter != _AllClients.end(); ++iter)
		{
			_FdRead.Add(iter->first);
			if (_MaxSocketID < iter->first)
				_MaxSocketID = iter->first;
		}

		_FdSetCache.Copy(_FdRead);

		_ClientChange = false;
	}
	else
	{
		_FdRead.Copy(_FdSetCache);
	}

	//设置_FdWrite
	bool bHasCanWriteClient = false;
	_FdWrite.Zero();
	for (std::map<SOCKET, std::shared_ptr<XClient>>::iterator iter = _AllClients.begin(); iter != _AllClients.end(); ++iter)
	{
		if (iter->second->GetFlush())
		{
			_FdWrite.Add(iter->first);
			bHasCanWriteClient = true;
		}
	}

	//设置1毫秒间隔，可以提高客户端连接select效率。
	//使用时间间隔可以提高客户端连接速度。使用阻塞模式更快。但此处不能使用组塞模式，需要执行定时检测任务。
	timeval tv = { 0, 0 };

	int nRet;
	if (bHasCanWriteClient)
		nRet = select((int)_MaxSocketID + 1, _FdRead.GetFdSet(), _FdWrite.GetFdSet(), nullptr, &tv);
	else
		nRet = select((int)_MaxSocketID + 1, _FdRead.GetFdSet(), nullptr, nullptr, &tv);

	if (SOCKET_ERROR == nRet)
	{
		XError("Select!\n");
		
	}

	RecvMsg();
	SendMsg();

	return nRet;
}

void XSelectServer::RecvMsg()
{
	fd_set* pSet = _FdRead.GetFdSet();

	//从客户端接收数据
#ifdef _WIN32

	for (int i = 0; i < (int)pSet->fd_count; ++i)
	{
		auto iter = _AllClients.find(pSet->fd_array[i]);
		if (iter != _AllClients.end())
		{
			int nRet = iter->second->RecvMsg();
			if (nRet != 0)
			{
				if (_pGlobalEventObj)
					_pGlobalEventObj->OnClientLeave(iter->second);

				_AllClients.erase(iter);
				_ClientChange = true;
				continue;
			}
		}
	}

#else

	for (std::map<SOCKET, std::shared_ptr<XClient>>::iterator iter = _AllClients.begin(); iter != _AllClients.end();)
	{
		if (_FdRead.Has(iter->first))
		{
			int ret = iter->second->RecvMsg();
			if (ret != 0)			//不等于0，ret为1说明接收缓冲区满了，主动断开连接。
			{
				if (_pGlobalEventObj)
					_pGlobalEventObj->OnClientLeave(iter->second);

				iter = _AllClients.erase(iter);
				_ClientChange = true;
				continue;
			}
		}

		++iter;
	}

#endif
}

void XSelectServer::SendMsg()
{	
	fd_set* pSet = _FdWrite.GetFdSet();

	//向客户端发送数据
#ifdef _WIN32

	for (int i = 0; i < (int)pSet->fd_count; ++i)
	{
		auto iter = _AllClients.find(pSet->fd_array[i]);
		if (iter != _AllClients.end())
		{
			int nRet = iter->second->SendMsg();
			if (nRet < 0)
			{
				if (_pGlobalEventObj)
					_pGlobalEventObj->OnClientLeave(iter->second);

				_AllClients.erase(iter);
				_ClientChange = true;
				continue;
			}
		}
	}

#else

	for (std::map<SOCKET, std::shared_ptr<XClient>>::iterator iter = _AllClients.begin(); iter != _AllClients.end();)
	{
		if (_FdWrite.Has(iter->first))
		{
			int ret = iter->second->SendMsg();
			if (ret < 0)			//小于0，ret为1时说明发送缓冲区为空，不应该断开。
			{
				if (_pGlobalEventObj)
					_pGlobalEventObj->OnClientLeave(iter->second);

				iter = _AllClients.erase(iter);
				_ClientChange = true;
				continue;
			}
		}

		++iter;
	}

#endif
}
