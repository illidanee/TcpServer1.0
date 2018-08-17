#ifndef __XTHREAD_H__
#define __XTHREAD_H__

#include "XSignal.h"
#include <functional>
#include <mutex>
#include <thread>

class XThread
{
	typedef std::function<void(XThread*)> PThreadFunc;

public:
	static void Sleep(time_t dt);

public:
	XThread();
	void Start(PThreadFunc pBegin = nullptr, PThreadFunc pRun = nullptr, PThreadFunc pEnd = nullptr);
	void Stop();
	void Exit();

	bool IsRun();

private:
	void OnWork();

private:
	bool _Run;											//当前线程是否正在运行
	XSignal _Signal;									//同步信号
	std::mutex _Mutex;									//安全锁

	PThreadFunc _pBegin;
	PThreadFunc _pRun;
	PThreadFunc _pEnd;
};

#endif