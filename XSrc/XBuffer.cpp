#include "XBuffer.h"

XBuffer::XBuffer(int nSize)
	:
	_pBuffer(nullptr),
	_nSize(nSize),
	_nOffset(0)
{
	Init();
}

XBuffer::~XBuffer()
{
	Done();
}

void XBuffer::Init()
{
	if (!_pBuffer)
	{
		_pBuffer = new char[_nSize];
	}
}

void XBuffer::Done()
{
	if (_pBuffer)
	{
		delete[] _pBuffer;
		_pBuffer = nullptr;
	}
}

int XBuffer::Recv(SOCKET socket)
{
	int num = _nSize - _nOffset;
	if (num <= 0)
	{
		//接收缓冲区已经满。
		return 1;
	}

	int size = recv(socket, _pBuffer + _nOffset, num, 0);
	if (size == -1)
	{
		//Socket错误。
		//XInfo("--%d \n", WSAGetLastError());
		return -1;
	}
	else if (size == 0)
	{
		//对端断开。
		return -2;
	}

	//正常接收。
	_nOffset += size;

	return 0;
}

int XBuffer::Send(SOCKET socket)
{
	if (_nOffset <= 0)
	{
		//发送缓冲区为空
		return 1;
	}

	int size = send(socket, _pBuffer, _nOffset, 0);
	if (size == -1)
	{
		//Socket错误。
		return -1;
	}

	//正常发送。
	_nOffset -= size;
	if (_nOffset > 0)
	{
		memcpy(_pBuffer + size, _pBuffer, _nOffset);
	}

	return 0;
}

int XBuffer::Push(MsgHeader* pMsg)
{
	if (_nOffset + pMsg->_MsgLength > _nSize)
	{
		//发送缓冲区满了。
		return -1;
	}

	memcpy(_pBuffer + _nOffset, pMsg, pMsg->_MsgLength);
	_nOffset += pMsg->_MsgLength;

	return 0;
}

int XBuffer::Pop()
{
	if (_nOffset < sizeof(MsgHeader))
	{
		//消息长度不够消息头。
		return -1;
	}

	MsgHeader* pMsg = (MsgHeader*)_pBuffer;
	if (_nOffset < pMsg->_MsgLength)
	{
		//消息长度不够消息体。
		return -2;
	}

	int length = pMsg->_MsgLength;
	if (_nOffset > pMsg->_MsgLength)
		memcpy(_pBuffer, _pBuffer + pMsg->_MsgLength, _nOffset - pMsg->_MsgLength);

	_nOffset -= length;

	return 0;
}

bool XBuffer::HasData()
{
	return _nOffset > 0;
}

bool XBuffer::HasMsg()
{
	if (_nOffset >= sizeof(MsgHeader))
	{
		MsgHeader* pMsg = (MsgHeader*)_pBuffer;
		if (_nOffset >= pMsg->_MsgLength)
			return true;
	}
	return false;
}

bool XBuffer::IsFull()
{
	return _nOffset >= _nSize / 2;
}

MsgHeader* XBuffer::Front()
{
	return (MsgHeader*)_pBuffer;
}
