//
// 201x-xx-xx, jjuiddong
// Serial Communication Class
//
// 2019-11-14
//		- refactoring
//
#pragma once


namespace common
{

	class cSerial
	{
	public:
		cSerial();
		~cSerial();

		bool Open(const int nPort = 2, const int nBaud = 9600);
		int ReadData(void *buffer, const int limit);
		int SendData(const char *buffer, const int size);
		int ReadDataWaiting();
		bool IsOpened() const;
		bool Close();


	protected:
		bool WriteCommByte(unsigned char ucByte);

		bool m_isOpened;
		HANDLE m_dev; // COM device handle
		OVERLAPPED m_read;
		OVERLAPPED m_write;
	};


	inline bool cSerial::IsOpened() const { return m_isOpened; }
}
