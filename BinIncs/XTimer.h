#ifndef __XTIMER_H__
#define __XTIMER_H__

#include <chrono>

class XTimer
{
public:
	static time_t GetTimeByMicroseconds();

public:
	int Init();
	int Done();

	void UpdateTime();
	double GetTime();

private:
	long long GetElapseTime();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _LastTimePoint;
};

#endif // ifndef
