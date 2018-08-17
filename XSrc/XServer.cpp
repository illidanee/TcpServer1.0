#include "XServer.h"



XServer::XServer()
	:
	_pGlobalEventObj(nullptr),
	_ID(0),
	_LastFrameTime(0),
	_CurFrameTime(0),
	_FrameTimeDelta(0),
	_ClientChange(false),
	_MaxSocketID(0)
{
}

XServer::~XServer()
{
}

void XServer::Init(XIGlobalEvent* pGlobalEventObj, int id)
{
	//设置全局事件对象
	_pGlobalEventObj = pGlobalEventObj;
	//设置服务线程ID
	_ID = id;

	//设置时间
	_LastFrameTime = XTimer::GetTimeByMicroseconds();
	_CurFrameTime = 0;
	_FrameTimeDelta = 0;

	//其他
	_FdRead.Zero();
	_FdWrite.Zero();
	_FdSetCache.Zero();
	_ClientChange = false;
	_MaxSocketID = 0;
}

void XServer::Done()
{
	////设置全局事件对象
	//_pGlobalEventObj = nullptr;
	////设置服务线程ID
	//_ID = 0;

	////设置时间
	//_LastFrameTime = XTimer::GetTimeByMicroseconds();
	//_CurFrameTime = 0;
	//_FrameTimeDelta = 0;

	////其他
	//_FdRead.Zero();
	//_FdWrite.Zero();
	//_FdSetCache.Zero();
	//_ClientChange = false;
	//_MaxSocketID = 0;
}

void XServer::Start()
{
	XInfo("    XReceiveServer<ID=%d>:Start() Begin\n", _ID);

	//开启任务线程
	_TaskServer.Start(_ID);

	//开始服务线程
	_Thread.Start(
		nullptr,
		[this](XThread* pThread) {
		OnRun(pThread);
	},
		nullptr
		);

	XInfo("    XReceiveServer<ID=%d>:Start() End\n", _ID);
}

void XServer::Stop()
{
	XInfo("    XReceiveServer<ID=%d>:Stop() Begin\n", _ID);

	//关闭服务线程
	_Thread.Stop();

	//关闭任务线程
	_TaskServer.Stop(_ID);

	//关闭所有客户端连接
	_AllClients.clear();
	_AllClientsCache.clear();

	XInfo("    XReceiveServer<ID=%d>:Stop() End\n", _ID);
}

void XServer::AddTask(std::function<void()> pTask)
{
	_TaskServer.AddTask(pTask);
}

XIGlobalEvent* XServer::GetGlobalObj()
{
	return _pGlobalEventObj;
}

void XServer::AddClient(const std::shared_ptr<XClient> pClient)
{
	std::lock_guard<std::mutex> lock(_AllClientsCacheMutex);
	_AllClientsCache.insert(std::pair<SOCKET, std::shared_ptr<XClient>>(pClient->GetSocket(), pClient));
}

int XServer::GetClientNum()
{
	return (int)_AllClients.size() + (int)_AllClientsCache.size();
}

void XServer::OnRun(XThread* pThread)
{
	XInfo("    XReceiveServer<ID=%d>:OnRun() Begin\n", _ID);

	while (pThread->IsRun())
	{
		int ret = 0;

		//必须在Continue之前，负责没有客户连接时会累积时间长度delta。
		UpdateFrameTimeDelta();

		//客户端状态检测。
		ret = CheckClientState();

		//检测是否有新的客户端以及当前是否有客户端。
		ret = CheckClientNum();
		if (ret < 0)
		{
			//当前没有客户端连接，减少CPU消耗。
			XThread::Sleep(1);
			continue;
		}

		//检测客户端，并处理。
		ret = DoSelect();
		if (ret < 0)
		{
			pThread->Exit();
			break;
		}
		else if (ret == 0)
		{
			continue;
		}

		//接收数据。
		RecvMsg();

		//发送数据。
		SendMsg();

		//处理数据。
		DoMsg();
	}

	XInfo("    XReceiveServer<ID=%d>:OnRun() End\n", _ID);
}

void XServer::UpdateFrameTimeDelta()
{
	//循环计时。
	_CurFrameTime = XTimer::GetTimeByMicroseconds();
	_FrameTimeDelta = _CurFrameTime - _LastFrameTime;
	_LastFrameTime = _CurFrameTime;
}

int XServer::CheckClientNum()
{
	//是否有新客户端加入？
	if (!_AllClientsCache.empty())
	{
		std::lock_guard<std::mutex> lock(_AllClientsCacheMutex);
		for (std::map<SOCKET, std::shared_ptr<XClient>>::iterator iter = _AllClientsCache.begin(); iter != _AllClientsCache.end(); ++iter)
		{
			_AllClients.insert(std::pair<SOCKET, std::shared_ptr<XClient>>(iter->first, iter->second));

			//不在这里回掉，使用在accept客户端并添加到缓冲区列表时调用。
			//if (_pGlobalEventObj)
			//	_pGlobalEventObj->OnClientJoin(iter->second);
		}
		_AllClientsCache.clear();

		_ClientChange = true;
	}

	//是否有客户端连接？
	if (_AllClients.empty())
	{
		return -1;
	}

	return 0;
}

int XServer::CheckClientState()
{
	int ret = 0;
	for (std::map<SOCKET, std::shared_ptr<XClient>>::iterator iter = _AllClients.begin(); iter != _AllClients.end(); )
	{
		//心跳检测。
		if (iter->second->CheckHeartTime(_FrameTimeDelta))
		{
			if (_pGlobalEventObj)
				_pGlobalEventObj->OnClientLeave(iter->second);

			iter = _AllClients.erase(iter);
			_ClientChange = true;

			++ret;
			continue;
		}
		
		////定时发送数据监测
		//iter->second->CheckSendTime(_FrameTimeDelta);

		////定量发送数据检测
		//iter->second->CheckSendNum();

		//立即发送
		if (iter->second->HasData())
			iter->second->Flush();

		++iter;
	}

	return ret;
}

int XServer::DoSelect()
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

	return nRet;
}

void XServer::RecvMsg()
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

void XServer::SendMsg()
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

void XServer::DoMsg()
{
	//处理客户端接收的数据
	for (std::map<SOCKET, std::shared_ptr<XClient>>::iterator iter = _AllClients.begin(); iter != _AllClients.end(); ++iter)
	{
		iter->second->DoMsg();
	}
}

