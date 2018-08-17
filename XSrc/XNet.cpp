#include "XNet.h"

void XNet::Go()
{
	static XNet obj;
}

int XNet::ReuseSocket(SOCKET s)
{
	int flag = 1;
	int nRet = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, sizeof(flag));
	if (SOCKET_ERROR == nRet) {
		XError("setsockopt\n");
		return -1;
	}
	return 0;
}

XNet::XNet()
{
//初始化网络环境
#ifdef _WIN32
	WORD wsaVersion = MAKEWORD(2, 2);
	WSADATA wsaData = {};
	int iRet = WSAStartup(wsaVersion, &wsaData);
	if (iRet)
	{
		XError("WSAStartup!\n");
	}
	else
	{
		XInfo("WSAStartup!\n");
	}
#endif
}

XNet::~XNet()
{
//销毁网络环境
#ifdef _WIN32
	int iRet = WSACleanup();
	if (SOCKET_ERROR == iRet)
	{
		XError("WSACleanup!\n");
	}
	else
	{
		XInfo("WSACleanup!\n");
	}
#endif
}