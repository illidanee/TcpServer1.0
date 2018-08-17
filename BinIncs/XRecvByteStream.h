#ifndef __XRECVBYTESTREAM_H__
#define __XRECVBYTESTREAM_H__

#include "XCommon.h"
#include "XByteStream.h"

class XRecvByteStream : public XByteStream
{
public:
	XRecvByteStream(MsgHeader* pMsgHeader);
};

#endif