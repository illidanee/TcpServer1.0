#include "XSignal.h"

XSignal::XSignal()
	:
	_WaitNum(0),
	_WakeNum(0)
{
}

void XSignal::Wait()
{
	std::unique_lock<std::mutex> lock(_Mutex);
	if (--_WaitNum < 0)
	{
		_CV.wait(lock, [this]()->bool { 
			return _WakeNum > 0;
		});
		--_WakeNum;
	}
}

void XSignal::Wake()
{
	std::lock_guard<std::mutex> lock(_Mutex);

	if (++_WaitNum <= 0)
	{
		++_WakeNum;
		_CV.notify_one();
	}

}