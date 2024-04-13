//
// 2024-04-12, jjuiddong
// Serial Communication Class
//	- upgrade from cSerial
//
#pragma once


namespace common
{

	class cSerial2
	{
	public:
		cSerial2();
		~cSerial2();

		bool Open(const int port = 2, const int baud = 9600);
		int ReadData(void *buffer, const uint limit);
		int SendData(const char *buffer, const uint size);
		int ReadDataWaiting();
		bool IsOpen() const;
		bool Close();


	protected:
		bool WriteCommByte(unsigned char data);


	public:
		HANDLE m_dev; // COM device handler
		OVERLAPPED m_read;
		OVERLAPPED m_write;
	};


	inline bool cSerial2::IsOpen() const { return m_dev? true : false; }
}
