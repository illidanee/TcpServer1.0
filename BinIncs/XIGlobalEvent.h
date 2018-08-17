#ifndef __XIGLOBALEVENT_H__
#define __XIGLOBALEVENT_H__

//全局事件接口。
struct MsgHeader;
class XClient;

class XIGlobalEvent
{
public:
	virtual void OnRunBegin() = 0;
	virtual void OnClientJoin(std::shared_ptr<XClient> pClient) = 0;
	virtual void OnClientLeave(std::shared_ptr<XClient> pClient) = 0;
	virtual void OnNetRecv(std::shared_ptr<XClient> pClient) = 0;
	virtual void OnNetSend(std::shared_ptr<XClient> pClient) = 0;
	virtual void OnNetMsgBegin(std::shared_ptr<XClient> pClient, MsgHeader* pMsgHeader) = 0;
	virtual void OnNetMsgEnd(std::shared_ptr<XClient> pClient, MsgHeader* pMsgHeader) = 0;
};

#endif