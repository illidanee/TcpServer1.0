#include "XTCPClient.h"

XTCPClient::XTCPClient()
	:
	_Client(nullptr),
	_bRun(false)
{
}

XTCPClient::~XTCPClient()
{
	 
}

//bool XTCPClient::Open()
//{
//	XNet::Go();
//
//	assert(_Client == nullptr);
//
//	//判断当前网络环境。
//	const char* pWWW = "ar.aoyikejivr.com";
//	
//	struct addrinfo hints;
//	memset(&hints, 0, sizeof(hints));
//	hints.ai_family = PF_UNSPEC;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_protocol = IPPROTO_TCP;
//
//	int error = getaddrinfo(pWWW, nullptr, &hints, &_Addr);
//	if (error)
//	{
//		XError("getaddrinfo!\n");
//		return false;
//	}
//
//	SOCKET _Socket = socket(_Addr->ai_family, _Addr->ai_socktype, _Addr->ai_protocol);
//	if (SOCKET_ERROR == _Socket)
//	{
//		XError("socket!\n");
//		return false;
//	}
//
//	_Client.reset(new XClient(this, this, _Socket));
//
//
//	return true;
//}

bool XTCPClient::Connect(const char* ip, unsigned short port)
{
	//优化：已经连接的时候，不在连接。此时如果执行connect会返回SOCKET_ERROR。
	if (_bRun)
		return true;

	//初始化网络
	XNet::Go();
	
	//获取网络类型 并根据网络类型是否 ipv4 转 ipv6。
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	
	int error = getaddrinfo(ip, "http", &hints, &_Addr);
	if (error)
	{
		//XError("getaddrinfo: error = %d; Infos = %s!\n", error, gai_strerror(error));
		XError("getaddrinfo!");
		return false;
	}

	//设置端口
	((sockaddr_in*)_Addr->ai_addr)->sin_port = htons(port);
	
	//创建Socket
	SOCKET _Socket = socket(_Addr->ai_family, _Addr->ai_socktype, _Addr->ai_protocol);
	if (SOCKET_ERROR == _Socket)
	{
		XError("socket!\n");
		return false;
	}

	XNet::ReuseSocket(_Socket);

	//设置指针对象。
	_Client.reset(new XClient(this, this, _Socket));

	int sinLen = sizeof(sockaddr_in);
	if (SOCKET_ERROR == connect(_Client->GetSocket(), _Addr->ai_addr, (int)_Addr->ai_addrlen))
	{
		XError("connect!\n");
		return false;
	}

	_bRun = true;

	OnClientJoin(_Client->GetSharedPtr());

	return true;
}

void XTCPClient::Disconnect()
{
	_bRun = false;

	OnClientLeave(_Client->GetSharedPtr());

	if (_Client != nullptr)
	{
		_Client.reset();
		_Client = nullptr;
	}
}

//void XTCPClient::Close()
//{
//	assert(_Client != nullptr);
//
//	if (_Client != nullptr)
//	{
//		_Client.reset();
//		_Client = nullptr;
//	}
//}

bool XTCPClient::IsRun()
{
	return _bRun;
}

void XTCPClient::OnRun()
{
	OnRunBegin();

	SOCKET _Socket = _Client->GetSocket();

	_FdRead.Zero();
	_FdWrite.Zero();

	_FdRead.Add(_Socket);

	timeval tv = { 0, 0 };

	int ret = 0;
	if (_Client->HasData())
	{
		_FdWrite.Add(_Socket);
		ret = select((int)_Socket + 1, _FdRead.GetFdSet(), _FdWrite.GetFdSet(), nullptr, &tv);
	}
	else
	{
		ret = select((int)_Socket + 1, _FdRead.GetFdSet(), nullptr, nullptr, &tv);
	}
	
	if (SOCKET_ERROR == ret)
	{
		XError("select!\n");
		Disconnect();
		return;
	}
	else if ( 0 == ret )
	{
		return;
	}

	//接收消息
	if (_FdRead.Has(_Socket))
	{
		int ret = _Client->RecvMsg();
		if (ret < 0)
		{
			Disconnect();
			return;
		}
	}

	//发送消息
	if (_FdWrite.Has(_Socket))
	{
		int ret = _Client->SendMsg();
		if (ret < 0)
		{
			Disconnect();
			return;
		}
	}

	//处理消息
	_Client->DoMsg();
}

int XTCPClient::SendData(MsgHeader* pHeader)
{
	return _Client->SendMsg(pHeader);
}

int XTCPClient::SendStream(XByteStream* pStream)
{
	return _Client->SendStream(pStream);
}