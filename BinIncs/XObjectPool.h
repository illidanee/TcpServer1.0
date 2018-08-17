#ifndef __XOBJECTPOOL_H__
#define __XOBJECTPOOL_H__

#include <assert.h>
#include <mutex>

#include "XLog.h"

template <size_t nCount, typename Type>
class XObjectPool
{
public:
	class XObjectBlock
	{
	public:
		size_t _nID;						//对象块ID。- id 为 -1 说明不在对象池中。
		size_t _nSize;						//对象块大小。
		XObjectPool* _pObjectPool;			//所属对象池。- _pMemoryPool 为 nullptr 说明不在对象池中。
		XObjectBlock* _pNext;				//下一个可用对象块地址。
		size_t _nRef;						//当前对象块被引用次数。
	};

private:
	size_t _nCount;					//对象块个数。
	size_t _nSize;					//对象块大小。
	char* _pBuffer;					//对象块首地址。
	XObjectBlock* _pCur;			//当前空闲对象块地址。

	std::mutex _mutex;				//锁。

public:
	XObjectPool()
	{
		_nCount = nCount;
		_nSize = sizeof(Type);
		_pBuffer = nullptr;
		_pCur = nullptr;
		Init();
	}

	~XObjectPool()
	{
		Done();
	}

	int Init()
	{
		assert(nullptr == _pBuffer);

		//XInfo(" - - - - - - - - - - - - - - - - - - - XObjectPool Init : _nCount = %d, _nSize = %d \n", (int)_nCount, (int)_nSize);

		std::lock_guard<std::mutex> lock(_mutex);

		size_t nBlockSize = sizeof(XObjectBlock) + _nSize;

		_pBuffer = new char[_nCount * nBlockSize];

		//初始化内存块。
		_pCur = (XObjectBlock*)_pBuffer;
		_pCur->_nID = 0;
		_pCur->_nSize = _nSize;
		_pCur->_pObjectPool = this;
		_pCur->_pNext = nullptr;
		_pCur->_nRef = 0;

		XObjectBlock* pLast = _pCur;
		for (int i = 1; i < _nCount; ++i)
		{
			XObjectBlock* pCur = (XObjectBlock*)((char*)_pBuffer + i * nBlockSize);
			pCur->_nID = i;
			pCur->_nSize = _nSize;
			pCur->_pObjectPool = this;
			pCur->_pNext = nullptr;
			pCur->_nRef = 0;

			pLast->_pNext = pCur;
			pLast = pCur;
		}

		return 0;
	}

	int Done()
	{
		assert(nullptr != _pBuffer);

		std::lock_guard<std::mutex> lock(_mutex);

		_nCount = 0;
		_nSize = 0;
		delete[] _pBuffer;
		_pBuffer = nullptr;

		return 0;
	}

	void* AllocObjectMemory(size_t nSize)
	{
		assert(nullptr != _pBuffer);

		XObjectBlock* pObjectBlock = nullptr;

		std::lock_guard<std::mutex> lock(_mutex);
		if (nullptr == _pCur)
		{
			//当前内存池没有可用内存块，在内存中申请。
			pObjectBlock = (XObjectBlock*)new char[sizeof(XObjectBlock) + nSize];
			pObjectBlock->_nID = -1;
			pObjectBlock->_nSize = nSize;
			pObjectBlock->_pObjectPool = nullptr;
			pObjectBlock->_pNext = nullptr;
			pObjectBlock->_nRef = 1;

			XInfo(" - - - - - - - - - - - - - - - - - - - AllocObjectMemory : Addr = %p, ID = %d, Size = %d \n", pObjectBlock, (int)pObjectBlock->_nID, (int)pObjectBlock->_nSize);
		}
		else
		{
			//使用内存池中的内存块。
			pObjectBlock = _pCur;
			pObjectBlock->_nRef += 1;
			_pCur = _pCur->_pNext;
		}

		return (char*)pObjectBlock + sizeof(XObjectBlock);
	}

	void FreeObjectMemory(void* pMem)
	{
		assert(nullptr != _pBuffer);
		assert(nullptr != pMem);

		XObjectBlock* pObjectBlock = (XObjectBlock*)((char*)pMem - sizeof(XObjectBlock));

		std::lock_guard<std::mutex> lock(_mutex);
		if (-1 == pObjectBlock->_nID)
		{
			XInfo(" - - - - - - - - - - - - - - - - - - - FreeObjectMemory : Addr = %p, ID = %d, Size = %d \n", pObjectBlock, (int)pObjectBlock->_nID, (int)pObjectBlock->_nSize);
			//使用系统申请的内存块。
			delete[] pObjectBlock;
		}
		else
		{
			//使用内存池中的内存块。 
			pObjectBlock->_pNext = _pCur;
			pObjectBlock->_nRef -= 1;
			_pCur = pObjectBlock;
		}
	}
};

#endif // ifndef
