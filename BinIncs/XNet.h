#ifndef __XNET_H__
#define __XNET_H__

#include "XCommon.h"

class XNet
{
public:
	static void Go();
	static int ReuseSocket(SOCKET s);

private:
	XNet();
	~XNet();
};

#endif