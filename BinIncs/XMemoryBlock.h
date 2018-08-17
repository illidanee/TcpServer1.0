#ifndef __XMEMORYBLOCK_H__
#define __XMEMORYBLOCK_H__

#include "XLog.h"

class XMemoryPool;

class XMemoryBlock
{
public:
	size_t _nID;						//内存块ID。- id 为 -1 说明不在内存池中。
	size_t _nSize;						//内存块大小。
	XMemoryPool* _pMemoryPool;			//所属内存池。- _pMemoryPool 为 nullptr 说明不在内存池中。
	XMemoryBlock* _pNext;				//下一个可用内存块地址。
	size_t _nRef;						//当前内存块被引用次数。
};

#endif