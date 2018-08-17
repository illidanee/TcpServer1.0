#ifndef __XISERVEREVENT_H__
#define __XISERVEREVENT_H__

#include <functional>

class XIServerEvent
{
public:
	virtual void AddTask(std::function<void()> pTask) = 0;
};

#endif