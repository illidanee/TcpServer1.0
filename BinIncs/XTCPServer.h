#ifndef __XSERVER_H__
#define __XSERVER_H__

#include "XCommon.h"
#include "XConfig.h"
#include "XIGlobalEvent.h"
#include "XNet.h"
#include "XServer.h"
#include "XClient.h"
#include "XThread.h"
#include <atomic>
#include <vector>

//监听Server类
class XTCPServer : public XIGlobalEvent
{
public:
	XTCPServer();
	~XTCPServer();

	int Init();
	int Done();
	int Start();
	int Stop();

public:
	virtual void OnMsg(std::shared_ptr<XClient> pClient, MsgHeader* pMsgHeader);

protected:
	virtual void OnRunBegin();
	virtual void OnClientJoin(std::shared_ptr<XClient> pClient);
	virtual void OnClientLeave(std::shared_ptr<XClient> pClient);
	virtual void OnNetRecv(std::shared_ptr<XClient> pClient);
	virtual void OnNetSend(std::shared_ptr<XClient> pClient);
	virtual void OnNetMsgBegin(std::shared_ptr<XClient> pClient, MsgHeader* pMsgHeader);
	virtual void OnNetMsgEnd(std::shared_ptr<XClient> pClient, MsgHeader* pMsgHeader);

//配置属性
private:
	const char* _IP;
	short _Port;
	int _LQN;
	int _ServerThreadSize;										//服务器线程数量
	int _ClientHeartTime;										//客户端心跳计时
	int _ClientSendTime;										//客户端发送计时
	int _ClientRecvBufferSize;									//客户端接收缓冲区大小
	int _ClientSendBufferSize;									//客户端发送缓冲区大小

//主要属性
private:
	SOCKET _Socket;												//服务器监听Socket
	std::vector<std::shared_ptr<XServer>> _AllServers;			//服务器信息

	XThread _Thread;											//任务线程
	XFdSet _FdRead;												
//统计属性
private:
	XTimer _Timer;												//计时器
	std::atomic_int _ClientNum;									//客户端计数器
	std::atomic_int _RecvNum;									//recv()函数调用计数
	std::atomic_int _SendNum;									//send()函数调用计数
	std::atomic_int _RecvMsgNum;								//接收数据包计数器
	std::atomic_int _SendMsgNum;								//处理数据包计数器

private:
	void Open();
	void Bind(const char* ip, unsigned short port);
	void Listen(int n);
	void Close();

	void Accept();

	void OnRun(XThread* pThread);
};


#endif