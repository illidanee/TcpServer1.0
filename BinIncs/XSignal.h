#ifndef __XSIGNAL_H__
#define __XSIGNAL_H__

#include <condition_variable>
#include <mutex>

class XSignal
{
public:
	XSignal();

	void Wait();
	void Wake();

private:
	std::condition_variable _CV;	//条件变量
	std::mutex _Mutex;				//锁

	int _WaitNum;					//等待计数
	int _WakeNum;					//唤醒计数
};

#endif