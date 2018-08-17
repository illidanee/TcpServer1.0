#include "XRecvByteStream.h"

XRecvByteStream::XRecvByteStream(MsgHeader* pMsgHeader)
	:
	XByteStream((char*)pMsgHeader, pMsgHeader->_MsgLength)
{
	SetReadOffset(sizeof(pMsgHeader->_MsgLength));
}