#ifndef __XTCPSELECTSERVER_H__
#define __XTCPSELECTSERVER_H__

#include "XTCPServer.h"
#include "XFdSet.h"
#include <atomic>
#include <vector>

//监听Server类
class XTCPSelectServer : public XTCPServer
{

	XFdSet _FdRead;												

	virtual void OnRun(XThread* pThread);
};


#endif