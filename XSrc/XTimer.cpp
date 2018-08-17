#include "XTimer.h"

time_t XTimer::GetTimeByMicroseconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

int XTimer::Init()
{
	UpdateTime();

	return 0;
}

int XTimer::Done()
{

	return 0;
}

void XTimer::UpdateTime()
{
	_LastTimePoint = std::chrono::high_resolution_clock::now();
}

double XTimer::GetTime()
{
	return GetElapseTime() * 0.000001;
}

long long XTimer::GetElapseTime()
{
	std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds t = std::chrono::duration_cast<std::chrono::microseconds>(end - _LastTimePoint);
	return t.count();
}

