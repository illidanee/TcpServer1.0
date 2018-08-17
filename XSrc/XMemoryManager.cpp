#include "XMemoryManager.h"
#include <stdlib.h>

void XMemoryManager::InitMemoryPools(size_t nBegin, size_t nEnd, XMemoryPool* pPool)
{
	for (size_t i = nBegin; i <= nEnd; ++i)
	{
		_AllMemoryPools[i] = pPool;
	}
}

XMemoryManager::XMemoryManager()
{
	InitMemoryPools(0 + 1, 16, &_MemoryPool16);
	InitMemoryPools(16 + 1, 32, &_MemoryPool32);
	InitMemoryPools(32 + 1, 64, &_MemoryPool64);
	InitMemoryPools(64 + 1, 128, &_MemoryPool128);
	InitMemoryPools(128 + 1, 256, &_MemoryPool256);
	InitMemoryPools(256 + 1, 512, &_MemoryPool512);
	InitMemoryPools(512 + 1, 1024, &_MemoryPool1024);
	InitMemoryPools(1024 + 1, 10240, &_MemoryPool10240);
	InitMemoryPools(10240 + 1, 20480, &_MemoryPool20480);
}

XMemoryManager::XMemoryManager(const XMemoryManager&)
{

}

XMemoryManager::~XMemoryManager()
{

}

XMemoryManager& XMemoryManager::operator=(const XMemoryManager&)
{

	return *this;
}

XMemoryManager& XMemoryManager::GetInstance()
{
	static XMemoryManager mgr;
	return mgr;
}

void* XMemoryManager::AllocMemory(size_t nSize)
{
	if (nSize <= _MAX_MEMORY_POOL_SIZE_)
	{
		//使用内存池。
		return _AllMemoryPools[nSize]->AllocMemory(nSize);
	}
	else
	{
		//没有对应的内存池可以使用。
		XMemoryBlock* pMemoryBlock = (XMemoryBlock*)malloc(sizeof(XMemoryBlock) + nSize);
		pMemoryBlock->_nID = -1;
		pMemoryBlock->_nSize = nSize;
		pMemoryBlock->_pMemoryPool = nullptr;
		pMemoryBlock->_pNext = nullptr;
		pMemoryBlock->_nRef = 1;

		XInfo(" - - - - - - - - - - - - - - - - - - - AllocMemory : Addr = %p, ID = %d, Size = %d \n", pMemoryBlock, (int)pMemoryBlock->_nID, (int)pMemoryBlock->_nSize);

		return (char*)pMemoryBlock + sizeof(XMemoryBlock);
	}
}

void XMemoryManager::FreeMemory(void* pMem)
{
	assert(nullptr != pMem);

	XMemoryBlock* pMemoryBlock = (XMemoryBlock*)((char*)pMem - sizeof(XMemoryBlock));
	
	if (pMemoryBlock->_pMemoryPool)
	{
		//使用内存池。
		pMemoryBlock->_pMemoryPool->FreeMemory(pMem);
	}
	else
	{
		XInfo(" - - - - - - - - - - - - - - - - - - - FreeMemory : Addr = %p, ID = %d, Size = %d \n", pMemoryBlock, (int)pMemoryBlock->_nID, (int)pMemoryBlock->_nSize);

		//没有内存池可以使用。
		free(pMemoryBlock);
	}
}