#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "XCommon.h"
#include "XIGlobalEvent.h"
#include "XIServerEvent.h"
#include "XNet.h"
#include "XClient.h"
#include "XSendByteStream.h"

#include "XFdSet.h"

//Client¿‡
class XTCPClient : public XIGlobalEvent, public XIServerEvent
{
public:
	XTCPClient();
	~XTCPClient();

	//bool Open();
	bool Connect(const char* ip, unsigned short port);
	void Disconnect();
	//void Close();

	bool IsRun();
	void OnRun();

	int SendData(MsgHeader* pHeader);
	int SendStream(XByteStream* pStream);

private:
	struct addrinfo* _Addr;

	std::shared_ptr<XClient> _Client;

	bool _bRun;

	XFdSet _FdRead;
	XFdSet _FdWrite;


};

#endif