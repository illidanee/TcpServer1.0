#ifndef __XMEMORYMANAGER_H__
#define __XMEMORYMANAGER_H__

#include "XMemoryPool.h"

#define _MAX_MEMORY_POOL_SIZE_ 20480

/****************************************************************************************************************
	Date   : 2018/07/23 11:01
	 
	Author : smile@illidan.org
	
	Brief  : 内存池中尽量不使用日志打印任务，防止隐式递归打印造成Crash。 
					日志打印时，需要使用Stl的队列，在队列中添加元素会有new操作。
					如果这个new的内存在内存池中不足，就会造成隐式递归。
					本程序中XLog中使用的是80内存，所以需要保证128大小的内存池有足够多。
****************************************************************************************************************/

class XMemoryManager
{
private:
	//配置的内存池
	XTMemoryPool<102400, 16>		_MemoryPool16;
	XTMemoryPool<102400, 32>		_MemoryPool32;
	XTMemoryPool<102400, 64>		_MemoryPool64;
	XTMemoryPool<102400, 128>		_MemoryPool128;
	XTMemoryPool<102400, 256>		_MemoryPool256;
	XTMemoryPool<102400, 512>		_MemoryPool512;
	XTMemoryPool<51200, 1024>		_MemoryPool1024;
	XTMemoryPool<51200, 10240>		_MemoryPool10240;
	XTMemoryPool<51200, 20480>		_MemoryPool20480;

	XMemoryPool* _AllMemoryPools[_MAX_MEMORY_POOL_SIZE_ + 1];

	void InitMemoryPools(size_t nBegin, size_t nEnd, XMemoryPool* pPool);

private:
	XMemoryManager();
	XMemoryManager(const XMemoryManager&);
	XMemoryManager& operator= (const XMemoryManager&);
	~XMemoryManager();

public:
	static XMemoryManager& GetInstance();

	void* AllocMemory(size_t nSize);
	void FreeMemory(void* pMem);
};

#endif