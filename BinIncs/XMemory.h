#ifndef __MEMORY_H__
#define __MEMORY_H__

#ifndef _WIN32
#include <stddef.h>
#endif

void* operator new(size_t nSize);
void operator delete(void* pMem) noexcept;

void* operator new[](size_t nSize);
void operator delete[](void* pMem) noexcept;

#endif