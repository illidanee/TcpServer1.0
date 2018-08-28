#include "XTCPSelectServer.h"

void XTCPSelectServer::OnRun(XThread* pThread)
{
	XInfo("---------------------------------------------------------------------------------------------------- XServer:OnRun() Begin\n");

	while (pThread->IsRun())
	{
		OnRunBegin();

		_FdRead.Zero();
		_FdRead.Add(_Socket);

		//设置10毫秒间隔，可以提高数据接受和发送select效率。
		timeval tv = { 0, 1 };
		int ret = select((int)_Socket + 1, _FdRead.GetFdSet(), NULL, NULL, &tv);
		if (SOCKET_ERROR == ret)
		{
			XError("<Socket=%d>:Select!\n", (int)_Socket);
			pThread->Exit();
			break;
		}
		else if (0 == ret)
		{
			continue;
		}

		if (_FdRead.Has(_Socket))
		{
			//_FdRead.Del(_Socket);
			Accept();
		}
	}

	XInfo("---------------------------------------------------------------------------------------------------- XServer:OnRun() End\n");
}




