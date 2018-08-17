#ifndef __XOBJECT_MANAGER_H__
#define __XOBJECT_MANAGER_H__

#include "XObjectPool.h"

template <size_t nCount, typename Type>
class XObject
{
public:
	void* operator new(size_t nSize)
	{
		return XObject::GetObjectPool().AllocObjectMemory(nSize);
	}

	void operator delete(void* pMem)
	{
		XObject::GetObjectPool().FreeObjectMemory(pMem);
	}

	void* operator new[](size_t nSize)
	{
		return XObject::GetObjectPool().AllocObjectMemory(nSize);
	}

	void operator delete[](void* pMem)
	{
		XObject::GetObjectPool().FreeObjectMemory(pMem);
	}

	template <typename... Args>
	static Type* CreateObject(Args... args)
	{
		Type* pObj = new Type(args...);
		return pObj;
	}

	static void DeleteObject(Type* pObj)
	{
		delete pObj;
	}

private:
	static XObjectPool<nCount, Type>& GetObjectPool()
	{
		static XObjectPool<nCount, Type> objPool;
		return objPool;
	}
};

#endif
