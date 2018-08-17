#include "XTCPServer.h"

XTCPServer::XTCPServer()
	:
	_Socket(INVALID_SOCKET),
	_IP(_IP_),
	_Port(_PORT_),
	_LQN(_LQN_),
	_ServerThreadSize(_XSERVER_THREAD_SIZE_),
	_ClientHeartTime(_XCLIENT_HEART_TIME_),
	_ClientSendTime(_XCLIENT_SEND_TIME_),
	_ClientRecvBufferSize(_XCLIENT_RECV_BUFFER_SIZE_),
	_ClientSendBufferSize(_XCLIENT_SEND_BUFFER_SIZE_)
{
}

XTCPServer::~XTCPServer()
{
}

int XTCPServer::Init()
{
	//获取服务器配置
	_IP = XConfig::Instance().GetStringArg("IP", _IP_);
	_Port = XConfig::Instance().GetIntArg("Port", _PORT_);
	_LQN = XConfig::Instance().GetIntArg("LQN", _LQN_);

	_ServerThreadSize = XConfig::Instance().GetIntArg("ServerThreadSize", _XSERVER_THREAD_SIZE_);
	_ClientHeartTime = XConfig::Instance().GetIntArg("ClientHeartTime", _XCLIENT_HEART_TIME_);
	_ClientSendTime = XConfig::Instance().GetIntArg("ClientSendTime", _XCLIENT_SEND_TIME_);
	_ClientRecvBufferSize = XConfig::Instance().GetIntArg("ClientRecvBufferSize", _XCLIENT_RECV_BUFFER_SIZE_);
	_ClientSendBufferSize = XConfig::Instance().GetIntArg("ClientSendBufferSize", _XCLIENT_SEND_BUFFER_SIZE_);

	XInfo("    Server Configure Infos: \n");
	XInfo("        %32s = %-32s\n", "IP", _IP);
	XInfo("        %32s = %-32d\n", "Port", _Port);
	XInfo("        %32s = %-32d\n", "Lqn", _LQN);
	XInfo("        %32s = %-32d\n", "ServerThreadSize", _ServerThreadSize);
	XInfo("        %32s = %-32d\n", "ClientHeartTime", _ClientHeartTime);
	XInfo("        %32s = %-32d\n", "ClientSendTime", _ClientSendTime);
	XInfo("        %32s = %-32d\n", "ClientRecvBufferSize", _ClientRecvBufferSize);
	XInfo("        %32s = %-32d\n", "ClientSendBufferSize", _ClientSendBufferSize);

	if (_IP && strcmp("Any", _IP) == 0)
	{
		_IP = nullptr;
	}

	//初始化其他
	_Timer.Init();
	_ClientNum = 0;
	_RecvNum = 0;
	_SendNum = 0;
	_RecvMsgNum = 0;
	_SendMsgNum = 0;

	return 0;
}

int XTCPServer::Done()
{
	_Timer.Done();

	return 0;
}

int XTCPServer::Start()
{
	XInfo("---------------------------------------------------------------------------------------------------- XServer:Start() Begin \n");

	//初始化网络环境
	XNet::Go();

	//初始化服务器
	Open();
	Bind(_IP, _Port);
	Listen(_LQN);

	//开启服务线程
	for (int i = 0; i < _ServerThreadSize; ++i)
	{
		//1-不使用对象池。
		//std::shared_ptr<XReceiveServer> pServer = std::make_shared<XReceiveServer>();
		//2-使用对象池。
		std::shared_ptr<XServer> pServer(new XServer());

		pServer->Init(this, i);
		pServer->Start();
		_AllServers.push_back(pServer);
	}

	//开始监听线程
	_Thread.Start(
		nullptr,
		[this](XThread* pThread) {
			OnRun(pThread);
		},
		nullptr
	);

	XInfo("---------------------------------------------------------------------------------------------------- XServer:Start() End \n");
	return 0;
}

int XTCPServer::Stop()
{
	XInfo("---------------------------------------------------------------------------------------------------- XServer:Stop() Begin \n");

	//关闭监听线程
	_Thread.Stop();

	//关闭服务线程
	for (auto iter : _AllServers)
	{	
		iter->Stop();
		iter->Done();
	}
	_AllServers.clear();

	//销毁服务器
	Close();

	XInfo("---------------------------------------------------------------------------------------------------- XServer:Stop() End \n");
	return 0;
}

void XTCPServer::OnMsg(std::shared_ptr<XClient> pClient, MsgHeader* pMsgHeader)
{

}

void XTCPServer::OnRunBegin()
{
	if (_Timer.GetTime() > 1.0)
	{
		XInfo("| ClientNum = %7d  | RecvNum = %7d  | SendNum = %7d  | RecvMsgNum = %7d  | SendMsgNum = %7d  |\n", (int)_ClientNum, (int)_RecvNum, (int)_SendNum, (int)_RecvMsgNum, (int)_SendMsgNum);
		_RecvNum = 0;
		_SendNum = 0;
		_RecvMsgNum = 0;
		_SendMsgNum = 0;
		_Timer.UpdateTime();
	}
}

void XTCPServer::OnClientJoin(std::shared_ptr<XClient> pClient)
{
	++_ClientNum;
}

void XTCPServer::OnClientLeave(std::shared_ptr<XClient> pClient)
{
	--_ClientNum;
}

void XTCPServer::OnNetRecv(std::shared_ptr<XClient> pClient)
{
	++_RecvNum;
}

void XTCPServer::OnNetSend(std::shared_ptr<XClient> pClient)
{
	++_SendNum;
}

//这里地方的pMsgHeader地址上的内存不安全。随时可能被释放。如果假如任务系统延迟处理会有危险。
void XTCPServer::OnNetMsgBegin(std::shared_ptr<XClient> pClient, MsgHeader* pMsgHeader)
{
	pClient->ResetHeartTime();

	++_RecvMsgNum;

	OnMsg(pClient, pMsgHeader);
}

void XTCPServer::OnNetMsgEnd(std::shared_ptr<XClient> pClient, MsgHeader* pMsgHeader)
{
	++_SendMsgNum;
}

void XTCPServer::Open()
{
	assert(INVALID_SOCKET == _Socket);

	_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _Socket)
	{
		XError("socket!\n");
	}
	else
	{
		XNet::ReuseSocket(_Socket);
		XInfo("<Socket=%d>:socket!\n", (int)_Socket);
	}
}

void XTCPServer::Bind(const char* ip, unsigned short port)
{
	assert(INVALID_SOCKET != _Socket);

	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
#ifdef _WIN32
	if (ip)
	{
		sin.sin_addr.S_un.S_addr = inet_addr(ip);
	}
	else
	{
		sin.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	}
#else
	if (ip)
	{
		sin.sin_addr.s_addr = inet_addr(ip);
	}
	else
	{
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
	}
#endif
	sin.sin_port = htons(port);
	int iRet = bind(_Socket, (sockaddr*)&sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == iRet)
	{
		XError("<Socket=%d>:bind!\n", (int)_Socket);
	}
	else
	{
		XInfo("<Socket=%d>:bind!\n", (int)_Socket);
	}
}

void XTCPServer::Listen(int n)
{
	assert(INVALID_SOCKET != _Socket);

	int iRet = listen(_Socket, n);
	if (SOCKET_ERROR == iRet)
	{
		XError("<Socket=%d>:listen!\n", (int)_Socket);
	}
	else
	{
		XInfo("<Socket=%d>:listen!\n", (int)_Socket);
	}
}

void XTCPServer::Close()
{
	assert(INVALID_SOCKET != _Socket);

	if (INVALID_SOCKET != _Socket)
	{
#ifdef _WIN32
		if (SOCKET_ERROR == closesocket(_Socket))
		{
			XError("<Socket=%d>:closesocket!\n", (int)_Socket);
		}
		else
		{
			XInfo("<Socket=%d>:closesocket!\n", (int)_Socket);
		}
#else
		if (SOCKET_ERROR == close(_Socket))
		{
			XError("<Socket=%d>:close!\n", (int)_Socket);
		}
		else
		{
			XInfo("<Socket=%d>:close!\n", (int)_Socket);
		}
#endif
		_Socket = INVALID_SOCKET;
	}
}

void XTCPServer::Accept()
{
	assert(INVALID_SOCKET != _Socket);

	sockaddr_in sinClient = {};
	int sinLen = sizeof(sockaddr_in);
#ifdef _WIN32
	SOCKET client = accept(_Socket, (sockaddr*)&sinClient, &sinLen);
#else
	SOCKET client = accept(_Socket, (sockaddr*)&sinClient, (socklen_t*)&sinLen);
#endif
	if (client == INVALID_SOCKET)
	{
		XError("<Socket=%d>:accept!\n", (int)_Socket);
	}
	else
	{
		if (_ClientNum >= _XFDSET_SIZE_)
		{
			const char* pAddr = inet_ntoa(sinClient.sin_addr);
			XWarn("Kill ip : %s \n", pAddr);

#ifdef _WIN32
			closesocket(client);
#else
			close(client);
#endif	
		}
		else
		{
			XNet::ReuseSocket(client);

			std::shared_ptr<XServer> pLessServer = _AllServers[0];
			for (auto pServer : _AllServers)
			{
				if (pLessServer->GetClientNum() > pServer->GetClientNum())
				{
					pLessServer = pServer;
				}
			}
			std::shared_ptr<XClient> pClient(new XClient(this, pLessServer.get(), client, _ClientHeartTime, _ClientSendTime, _ClientRecvBufferSize, _ClientSendBufferSize));
			pLessServer->AddClient(std::shared_ptr<XClient>(pClient));

			//在这里调用客户端加入的回调函数。
			//	当Accept后，将client加入到缓冲区列表中时就认为客户端已经连接服务器了。
			//	如果当client从缓冲区列表中加入到正式客户端列表中时调用，会产生计数延迟。
			OnClientJoin(pClient);
		}
	}
}

void XTCPServer::OnRun(XThread* pThread)
{
	XInfo("---------------------------------------------------------------------------------------------------- XServer:OnRun() Begin\n");

	while (pThread->IsRun())
	{
		OnRunBegin();

		_FdRead.Zero();
		_FdRead.Add(_Socket);

		//设置10毫秒间隔，可以提高数据接受和发送select效率。
		timeval tv = { 0, 1 };
		int ret = select((int)_Socket + 1, _FdRead.GetFdSet(), NULL, NULL, &tv);
		if (SOCKET_ERROR == ret)
		{
			XError("<Socket=%d>:Select!\n", (int)_Socket);
			pThread->Exit();
			break;
		}
		else if (0 == ret)
		{
			continue;
		}

		if (_FdRead.Has(_Socket))
		{
			//_FdRead.Del(_Socket);
			Accept();
		}
	}

	XInfo("---------------------------------------------------------------------------------------------------- XServer:OnRun() End\n");
}




