#include "XLog.h"

XLog& XLog::GetInstance()
{
	static XLog ins;
	return ins;
}

void XLog::SetFileName(const char* pFileName, const char* pMode)
{
	XLog& log = GetInstance();

	if (log._File)
	{
		fclose(log._File);
		log._File = nullptr;
	}

	//计算当前时间
	std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(t);
	std::tm* ttt = std::localtime(&tt);

	char file[1024] = {};
	sprintf(file, "%s__[%04d-%02d-%02d_%02d-%02d-%02d]", pFileName, ttt->tm_year + 1900, ttt->tm_mon + 1, ttt->tm_mday, ttt->tm_hour, ttt->tm_min, ttt->tm_sec);

	log._File = fopen(file, pMode);
	
	assert(log._File != nullptr);
}

XLog::XLog()
	:
	_File(nullptr),
	_Run(false),
	_WaitNum(0),
	_WakeNum(0)
{
	Start();
}

XLog::XLog(const XLog& that)
{
	//禁止
}

XLog& XLog::operator=(const XLog& that)
{
	//禁止
	return *this;
}

XLog::~XLog()
{
	Stop();

	if (_File)
		fclose(_File);
}

void XLog::Start()
{
	std::lock_guard<std::mutex> lock(_ThreadlMutex);

	if (!_Run)
	{
		//设置状态
		_Run = true;

		//启动服务器进程
		std::thread t(std::mem_fn(&XLog::Run), this);
		t.detach();	
	}
}

void XLog::Stop()
{
	std::lock_guard<std::mutex> lock(_ThreadlMutex);

	if (_Run)
	{
		//设置状态
		_Run = false;

		//信号-等待
		std::unique_lock<std::mutex> lock(_SignalMutex);
		if (--_WaitNum < 0)
		{
			_CV.wait(lock, [this]()->bool {
				return _WakeNum-- > 0;
			});
		}
	}
}

void XLog::Run()
{
	while (_Run)
	{
		//从缓冲区中取数据
		if (!_TasksCache.empty())
		{
			std::lock_guard<std::mutex> lock(_TasksCacheMutex);
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

	//信号-发送
	std::lock_guard<std::mutex> lock(_SignalMutex);
	if (++_WaitNum <= 0)
	{
		++_WakeNum;
		_CV.notify_one();
	}
}

void XLog::AddTask(std::function<void()> pTask)
{
	std::lock_guard<std::mutex> lock(_TasksCacheMutex);
	_TasksCache.push_back(pTask);
}