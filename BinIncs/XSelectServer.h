#ifndef __XSELECTSERVER_H__
#define __XSELECTSERVER_H__

#include "XServer.h"
#include "XFdSet.h"

class XSelectServer : public XServer
{
	virtual int DoNetEvent();

	void RecvMsg();
	void SendMsg();


	//此集合占用大量的内存,使对象大小变大。
	XFdSet _FdRead;
	XFdSet _FdWrite;
	XFdSet _FdSetCache;												//fd_set 缓存
	SOCKET _MaxSocketID;											//客户端最大Socket
};

#endif