#include "XByteStream.h"

XByteStream::XByteStream(char* pBuffer, int nSize)
	:
	_pBuffer(nullptr),
	_nSize(nSize),
	_nReadOffset(0),
	_nWriteOffset(0)
{
	//此处必须重新申请内存保存栈空间的地址内容，否则如果_pBuffer指向的栈空间被释放则出现数据错误。3天解决。
	//类引用外部指针必须保证其不被释放，最好自己申请内存空间。
	_pBuffer = new char[_nSize];
	memcpy(_pBuffer, pBuffer, nSize);
}

XByteStream::XByteStream(int nSize)
	:
	_pBuffer(nullptr),
	_nSize(nSize),
	_nReadOffset(0),
	_nWriteOffset(0)
{
	_pBuffer = new char[_nSize];
}

XByteStream::~XByteStream()
{
	delete[] _pBuffer;
}

bool XByteStream::ReadInt8(int8_t& num)
{
	return Read(num);
}

bool XByteStream::ReadInt16(int16_t& num)
{
	return Read(num);
}

bool XByteStream::ReadInt32(int32_t& num)
{
	return Read(num);
}

bool XByteStream::ReadInt64(int64_t& num)
{
	return Read(num);
}

bool XByteStream::ReadFloat(float& num)
{
	return Read(num);
}

bool XByteStream::ReadDouble(double& num)
{
	return Read(num);
}

bool XByteStream::WriteInt8(int8_t num)
{
	return Write(num);
}

bool XByteStream::WriteInt16(int16_t num)
{
	return Write(num);
}

bool XByteStream::WriteInt32(int32_t num)
{
	return Write(num);
}

bool XByteStream::WriteInt64(int64_t num)
{
	return Write(num);
}

bool XByteStream::WriteFloat(float num)
{
	return Write(num);
}

bool XByteStream::WriteDouble(double num)
{
	return Write(num);
}

char* XByteStream::GetBuffer()
{
	return _pBuffer;
}

int XByteStream::GetReadOffset()
{
	return _nReadOffset;
}

int XByteStream::GetWriteOffset()
{
	return _nWriteOffset;
}

void XByteStream::SetReadOffset(int offset)
{
	_nReadOffset = offset;
}

void XByteStream::SetWriteOffset(int offset)
{
	_nWriteOffset = offset;
}