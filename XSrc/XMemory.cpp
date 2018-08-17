#include "XMemory.h"
#include "XMemoryManager.h"

void* operator new(size_t nSize)
{
	return XMemoryManager::GetInstance().AllocMemory(nSize);
}

void operator delete(void* pMem) noexcept
{
	XMemoryManager::GetInstance().FreeMemory(pMem);
}

void* operator new[](size_t nSize)
{
	return XMemoryManager::GetInstance().AllocMemory(nSize);
}

void operator delete[](void* pMem) noexcept
{
	XMemoryManager::GetInstance().FreeMemory(pMem);
}