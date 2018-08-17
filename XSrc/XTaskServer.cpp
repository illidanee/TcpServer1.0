#include "XTaskServer.h"

XTaskServer::XTaskServer()
{
}

XTaskServer::~XTaskServer()
{
}

int XTaskServer::Start(int id)
{
	XInfo("        XTaskServer<ID=%d>:Start() Begin\n", id);

	//开启服务线程
	_Thread.Start(
		nullptr, 
		[this](XThread* pThread) {
			OnRun(pThread);
		}, 
		nullptr);

	XInfo("        XTaskServer<ID=%d>:Start() End\n", id);
	return 0;
}

int XTaskServer::Stop(int id)
{
	XInfo("        XTaskServer<ID=%d>:Stop() Begin\n", id);

	//关闭服务线程
	_Thread.Stop();

	XInfo("        XTaskServer<ID=%d>:Stop() End\n", id);
	return 0;
}

int XTaskServer::OnRun(XThread* pThread)
{
	XInfo("        XTaskServer<ID=%d>:OnRun() Begin\n", -1);

	while (pThread->IsRun())
	{
		//从缓冲区中取数据
		if (!_TasksCache.empty())
		{
			std::lock_guard<std::mutex> lock(_TaskSCacheMutex);
			for (std::list<std::function<void()>>::iterator iter = _TasksCache.begin(); iter != _TasksCache.end(); ++iter)
			{
				_Tasks.push_back(*iter);
			}
			_TasksCache.clear();
		}

		//没有数据。
		if (_Tasks.empty())
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1));
			continue;
		}

		//处理任务
		for (std::list<std::function<void()>>::iterator iter = _Tasks.begin(); iter != _Tasks.end(); ++iter)
		{
			(*iter)();
		}

		//清空任务
		_Tasks.clear();
	}

	//处理缓存中的剩余数据。
	for (std::list<std::function<void()>>::iterator iter = _TasksCache.begin(); iter != _TasksCache.end(); ++iter)
	{
		(*iter)();
	}

	//清空任务
	_TasksCache.clear();

	XInfo("        XTaskServer<ID=%d>:OnRun() End\n", -1);

	return 0;
}

void XTaskServer::AddTask(std::function<void()> pTask)
{
	std::lock_guard<std::mutex> lock(_TaskSCacheMutex);
	_TasksCache.push_back(pTask);
}