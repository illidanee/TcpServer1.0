#ifndef __XCLIENT_H__
#define __XCLIENT_H__

#include "XCommon.h"
#include "XIGlobalEvent.h"
#include "XIServerEvent.h"
#include "XBuffer.h"
#include "XByteStream.h"

//客户端信息类
class XClient : public XObject<10240, XClient>, public std::enable_shared_from_this<XClient>
{
public:
	XClient(XIGlobalEvent* pGlobalObj, XIServerEvent* pServerObj, SOCKET client, int nHeartTime = _XCLIENT_HEART_TIME_, int nSendTime = _XCLIENT_SEND_TIME_, int nRecvBufferSize = _XCLIENT_RECV_BUFFER_SIZE_, int nSendBufferSize = _XCLIENT_SEND_BUFFER_SIZE_);
	~XClient();

	int RecvMsg();
	int SendMsg();
	void DoMsg();

	int SendMsg(MsgHeader* pHeader);
	int SendStream(XByteStream* pByteStream);

	bool HasData();
	bool HasMsg();
	bool IsBufferFull();

	//心跳
	void ResetHeartTime();
	bool CheckHeartTime(time_t t);

	//定时发送数据
	void ResetSendTime();
	void CheckSendTime(time_t t);

	//定量发送数据
	void CheckSendNum();

	//是否需要发送数据
	void ResetFlush();
	void Flush();
	bool GetFlush();

public:
	XIGlobalEvent* GetGlobalObj();
	XIServerEvent* GetServerObj();

	//获取自身的shared_prt
	std::shared_ptr<XClient> GetSharedPtr();

	SOCKET GetSocket() { return _Socket; }

private:
	XIGlobalEvent* _pGlobalObj;					//全局对象
	XIServerEvent* _pServerObj;					//服务对象

	SOCKET _Socket;								//客户端Socket

	time_t _CurHeartTime;						//心跳计时器
	time_t _HeartTime;							//心跳总时间
	time_t _CurSendTime;						//发送计时器
	time_t _SendTime;							//发送总时间
	bool _bFlush;								//是否需要马上发送数据

	XBuffer _RecvBuffer;						//接收缓冲区对象
	XBuffer _SendBuffer;						//发送缓冲区对象

public:
	int _CurMsgID;								//当前消息ID
};


#endif