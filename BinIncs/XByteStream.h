#ifndef __XBYTESTREAM_H__
#define __XBYTESTREAM_H__

#include <stdint.h>
#include <memory.h>

class XByteStream
{
public:
	XByteStream(char* pBuffer, int nSize);
	XByteStream(int nSize);
	~XByteStream();

	//Read
	template <typename T>
	bool Read(T& num)
	{
		int nSize = sizeof(T);
		if (_nReadOffset + nSize <= _nSize)
		{
			memcpy(&num, _pBuffer + _nReadOffset, nSize);
			_nReadOffset += nSize;
			return true;
		}

		return false;
	}

	template <typename T>
	bool OnlyRead(T& num)
	{
		int nSize = sizeof(T);
		if (_nReadOffset + nSize <= _nSize)
		{
			memcpy(&num, _pBuffer + _nReadOffset, nSize);
			//_nReadOffset += nSize;
			return true;
		}

		return false;
	}

	template <typename T>
	int ReadArray(T* pBuffer, int nSize)
	{
		int32_t nLength;
		bool ret = OnlyRead(nLength);
		if (ret && nLength < nSize)
		{
			if (_nReadOffset + sizeof(int32_t) + nLength <= _nSize)
			{
				_nReadOffset += sizeof(int32_t);
				memcpy(pBuffer, _pBuffer + _nReadOffset, nLength);
				_nReadOffset += nLength;
				return nLength;
			}
		}

		return 0;
	}

	bool ReadInt8(int8_t& num);
	bool ReadInt16(int16_t& num);
	bool ReadInt32(int32_t& num);
	bool ReadInt64(int64_t& num);
	bool ReadFloat(float& num);
	bool ReadDouble(double& num);

	//Write
	template <typename T>
	bool Write(T num)
	{
		int nSize = sizeof(T);
		if (_nWriteOffset + nSize <= _nSize)
		{
			memcpy(_pBuffer + _nWriteOffset, &num, nSize);
			_nWriteOffset += nSize;
			return true;
		}

		return false;
	}

	template <typename T>
	int WriteArray(T* pBuffer, int nSize)
	{
		if (_nWriteOffset + sizeof(int32_t) + nSize <= _nSize)
		{
			WriteInt32(nSize);
			memcpy(_pBuffer + _nWriteOffset, pBuffer, nSize);
			_nWriteOffset += nSize;
			return nSize;
		}

		return 0;
	}

	bool WriteInt8(int8_t num);
	bool WriteInt16(int16_t num);
	bool WriteInt32(int32_t num);
	bool WriteInt64(int64_t num);
	bool WriteFloat(float num);
	bool WriteDouble(double num);

	char* GetBuffer();

	int GetReadOffset();
	int GetWriteOffset();
	void SetReadOffset(int offset);
	void SetWriteOffset(int offset);

private:
	char* _pBuffer;
	int _nSize;

	int _nReadOffset;
	int _nWriteOffset;
};

#endif