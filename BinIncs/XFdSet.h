#ifndef __XFDSET_H__
#define __XFDSET_H__

#define _XFDSET_SIZE_ 10240						//最大连接数 - Windows下需要与FD_SETSIZE大小相等。

#include "XCommon.h"

class XFdSet
{
public:
	XFdSet();
	~XFdSet();
	
	void Copy(XFdSet& set);

	void Zero();
	void Add(SOCKET s);
	void Del(SOCKET s);
	int Has(SOCKET s);

	fd_set* GetFdSet();

private:
	size_t _nSize;
	fd_set* _pFdSet;
};

#endif