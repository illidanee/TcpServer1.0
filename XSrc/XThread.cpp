#include "XThread.h"

XThread::XThread()
	:
	_Run(false),
	_pBegin(nullptr),
	_pRun(nullptr),
	_pEnd(nullptr)
{
}

void XThread::Start(PThreadFunc pBegin, PThreadFunc pRun, PThreadFunc pEnd)
{
	std::lock_guard<std::mutex> lock(_Mutex);

	if (!_Run)
	{
		if (pBegin)
			_pBegin = pBegin;
		if (pRun)
			_pRun = pRun;
		if (pEnd)
			_pEnd = pEnd;

		_Run = true;

		std::thread t(std::mem_fn(&XThread::OnWork), this);
		t.detach();
	}
}

void XThread::Stop()
{
	std::lock_guard<std::mutex> lock(_Mutex);

	if (_Run)
	{
		_Run = false;
		_Signal.Wait();
	}
}

void XThread::Exit()
{
	std::lock_guard<std::mutex> lock(_Mutex);
	if (_Run)
	{
		_Run = false;
	}
}

bool XThread::IsRun()
{
	return _Run == true;
}

void XThread::Sleep(time_t dt)
{
	std::chrono::microseconds t(dt);
	std::this_thread::sleep_for(t);
}

void XThread::OnWork()
{
	if (_pBegin)
		_pBegin(this);

	if (_pRun)
		_pRun(this);

	if (_pEnd)
		_pEnd(this);

	_Run = false;

	_Signal.Wake();
}