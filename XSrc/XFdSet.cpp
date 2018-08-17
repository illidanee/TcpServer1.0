#include "XFdSet.h"

XFdSet::XFdSet()
{
#ifdef _WIN32
	_nSize = sizeof(u_int) + sizeof(SOCKET) * _XFDSET_SIZE_;
#else
	_nSize = _XFDSET_SIZE_ / (8 * sizeof(char));
#endif
	_pFdSet = (fd_set*)new char[_nSize];
	memset(_pFdSet, 0, _nSize);
}

XFdSet::~XFdSet()
{
	delete[] _pFdSet;
	_pFdSet = nullptr;
}

void XFdSet::Copy(XFdSet& set)
{
	memcpy(_pFdSet, set._pFdSet, set._nSize);
}

void XFdSet::Zero()
{
#if _WIN32
	FD_ZERO(_pFdSet);
#else
	memset(_pFdSet, 0, _nSize);
#endif
}

void XFdSet::Add(SOCKET s)
{
#ifdef _WIN32
	FD_SET(s, _pFdSet);
#else
	if (s < _XFDSET_SIZE_)
	{
		FD_SET(s, _pFdSet);
	}
#endif
}

void XFdSet::Del(SOCKET s)
{
#ifdef _WIN32
	FD_CLR(s, _pFdSet);
#else
	if (s < _XFDSET_SIZE_)
	{
		FD_CLR(s, _pFdSet);
	}
#endif
}

int XFdSet::Has(SOCKET s)
{
	return FD_ISSET(s, _pFdSet);
}

fd_set* XFdSet::GetFdSet()
{
	return _pFdSet;
}