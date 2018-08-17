#include "XClient.h"

XClient::XClient(XIGlobalEvent* pGlobalObj, XIServerEvent* pServerObj, SOCKET client, int nHeartTime, int nSendTime, int nRecvBufferSize, int nSendBufferSize)
	:
	_pGlobalObj(pGlobalObj),
	_pServerObj(pServerObj),
	_Socket(client),
	_CurHeartTime(0),
	_HeartTime(nHeartTime),
	_CurSendTime(0),
	_SendTime(nSendTime),
	_bFlush(false),
	_RecvBuffer(nRecvBufferSize),
	_SendBuffer(nSendBufferSize),
	_CurMsgID(0)
{
}

XClient::~XClient()
{
#ifdef _WIN32
	shutdown(_Socket, SD_BOTH);
	closesocket(_Socket);
#else
	shutdown(_Socket, SHUT_RDWR);
	close(_Socket);
#endif
}

int XClient::RecvMsg()
{
	int nRet = _RecvBuffer.Recv(_Socket);

	if (_pGlobalObj)
		_pGlobalObj->OnNetRecv(shared_from_this());

	return nRet;
}

int XClient::SendMsg()
{
	int nRet = _SendBuffer.Send(_Socket);

	//发送了数据
	if (_pGlobalObj)
		_pGlobalObj->OnNetSend(shared_from_this());

	ResetSendTime();
	ResetFlush();

	return nRet;
}

void XClient::DoMsg()
{
	while (_RecvBuffer.HasMsg())
	{
		if (_pGlobalObj)
			_pGlobalObj->OnNetMsgBegin(shared_from_this(), _RecvBuffer.Front());

		_RecvBuffer.Pop();
	}
}

int XClient::SendMsg(MsgHeader* pHeader)
{
	int nRet = _SendBuffer.Push(pHeader);

	if (_pGlobalObj)
		_pGlobalObj->OnNetMsgEnd(shared_from_this(), pHeader);

	return nRet;
}

int XClient::SendStream(XByteStream* pByteStream)
{
	return SendMsg((MsgHeader*)pByteStream->GetBuffer());
}

bool XClient::HasData()
{
	return _SendBuffer.HasData();
}

bool XClient::HasMsg()
{
	return _SendBuffer.HasMsg();
}

bool XClient::IsBufferFull()
{
	return _SendBuffer.IsFull();
}

void XClient::ResetHeartTime()
{
	_CurHeartTime = 0;
}

bool XClient::CheckHeartTime(time_t t)
{
	_CurHeartTime += t;
	if (_CurHeartTime >= _HeartTime)
	{
		return true;
	}

	return false;
}

void XClient::ResetSendTime()
{
	_CurSendTime = 0;
}

void XClient::CheckSendTime(time_t t)
{
	//只有当客户端缓冲区里有数据时才计时。
	if (HasData())
	{
		_CurSendTime += t;
		if (_CurSendTime >= _SendTime)
		{
			_bFlush = true;

			//XInfo("  -----------------CheckSendTime ...\n");
		}
	}
}

void XClient::CheckSendNum()
{
	//当发送缓冲区的数据大于一半时，认为需要马上发送了。防止缓冲满。
	if (IsBufferFull())
	{
		_bFlush = true;

		//XInfo("  -----------------CheckSendNum ...\n");
	}	
}

void XClient::ResetFlush()
{
	_bFlush = false;
}

void XClient::Flush()
{
	_bFlush = true;
}

bool XClient::GetFlush()
{
	return _bFlush;
}

XIGlobalEvent* XClient::GetGlobalObj()
{
	return _pGlobalObj;
}

XIServerEvent* XClient::GetServerObj()
{
	return _pServerObj;
}

std::shared_ptr<XClient> XClient::GetSharedPtr()
{
	return shared_from_this();
}
