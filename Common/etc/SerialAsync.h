//
// 2015-12-17, jjuiddong
//	비동기 시리얼 통신
//	현재 Send 정보만 처리한다.
//
// 2019-11-15, jjuiddong
//		- asynchronous send queue
//		- asynchronous receive queue
//
//
#pragma once

#include "BufferedSerial.h"


namespace common
{

	class cSerialAsync
	{
	public:
		cSerialAsync();
		virtual ~cSerialAsync();

		bool Open(const int portNum, const int baudRate
			, const char delimeter='\n');
		int SendData(BYTE *buffer, const int size);
		uint RecvData(BYTE *buffer, const int size);
		void SetDelimeter(const char delimeter);
		bool IsOpen() const;
		void Close();


	public:
		static unsigned SerialThreadFunction(cSerialAsync *asyncSer);


	public:
		enum { BUFLEN = 512, MAX_BUFFERSIZE = 2048 };
		enum class eState { DISCONNECT, CONNECT };

		eState m_state;
		cCircularQueue<char, MAX_BUFFERSIZE> m_sndQ; // send queue
		cCircularQueue<char, MAX_BUFFERSIZE> m_rcvQ; // receive queue
		cBufferedSerial m_serial;
		char m_delimeter;
		int m_sleepMillis; // default = 10

		std::thread m_thread;
		CriticalSection m_cs;
	};

}

