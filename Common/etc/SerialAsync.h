//
//  2015-12-17, jjuiddong
//
// 비동기 시리얼 통신
// 현재 Send 정보만 처리한다.
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

		bool Open(const int portNum, const int baudRate);
		bool IsOpen();
		int SendData(BYTE *buffer, const int bufferLen);
		void Close();


	public:
		static unsigned SerialThreadFunction(cSerialAsync *serial);


	public:
		enum { BUFLEN = 512, };
		bool m_isConnect;
		char m_buffer[BUFLEN];
		int m_bufferLen;
		bool m_isSendData;
		cBufferedSerial m_serial;
		int m_sleepMillis; // default = 10

		std::thread m_thread;
		CriticalSection m_cs;
		bool m_threadLoop;
	};

}

