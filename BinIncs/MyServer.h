#ifndef __MYSERVER_H__
#define __MYSERVER_H__

#include "XCommon.h"
#include "XTCPSelectServer.h"
#include "XRecvByteStream.h"
#include "XSendByteStream.h"
#include "XMariaDB.h"
#include <atomic>
#include <functional>

//自定义Server
class MyServer : public XTCPSelectServer
{
public:
	MyServer();
	~MyServer();

	virtual void OnMsg(std::shared_ptr<XClient> pClient, MsgHeader* pMsgHeader);
};

#endif
