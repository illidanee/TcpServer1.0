#ifndef __XMARIADB_H__
#define __XMARIADB_H__

#include "XMariaDBConnect.h"
#include "XConfig.h"

#include <list>

class XMariaDB
{
public:
	static XMariaDB& GetInstance();

	bool HasConnect();
	XMariaDBConnect* GetConnect();
	void PushConnect(XMariaDBConnect* pConnect);

private:
	XMariaDB();
	~XMariaDB();

	void Init();
	void Done();

private:
	std::list<XMariaDBConnect*> _Connects;
	std::mutex _Mutex;
};

#endif