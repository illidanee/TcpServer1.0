#ifndef __XBUFFER_H__
#define __XBUFFER_H__

#include "XCommon.h"

class XBuffer : public XObject<20480, XBuffer>
{
public:
	XBuffer(int nSize);
	~XBuffer();

	void Init();
	void Done();

	int Recv(SOCKET socket);
	int Send(SOCKET socket);

	int Push(MsgHeader* pMsg);
	int Pop();
	bool HasData();					//缓冲区中有数据。
	bool HasMsg();					//缓冲区中有一条完整消息数据。
	bool IsFull();					//缓冲区是否已满。
	MsgHeader* Front();

private:
	char* _pBuffer;					//缓冲区
	int _nSize;						//缓冲区大小
	int _nOffset;					//缓冲区位置偏移
};

#endif