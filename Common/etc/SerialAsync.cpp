
#include "stdafx.h"
#include "SerialAsync.h"
#include <process.h>

using namespace common;


unsigned WINAPI SerialThreadFunction(void* arg);


cSerialAsync::cSerialAsync()
	: m_isConnect(false)
	, m_isSendData(false)
	, m_sleepMillis(10)
	, m_handle(NULL)
	, m_threadLoop(true)
{
	InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 0x00000400);
}

cSerialAsync::~cSerialAsync()
{
	Close();

	DeleteCriticalSection(&m_CriticalSection);
}


bool cSerialAsync::Open(const int portNum, const int baudRate)
{
	RETV(m_serial.IsOpened(), true);
	if (!m_serial.Open(portNum, baudRate))
		return false;
	
	m_isConnect = true;
	m_threadLoop = true;
	m_handle = (HANDLE)_beginthreadex(NULL, 0, SerialThreadFunction, this, 0, (unsigned*)&m_threadId);

	return true;
}


bool cSerialAsync::IsOpen()
{
	return m_isConnect;
}


int cSerialAsync::SendData(BYTE *buffer, const int bufferLen)
{
	if (BUFLEN > bufferLen)
	{
		cAutoCS cs(m_CriticalSection);
		m_bufferLen = bufferLen;
		memcpy(m_buffer, buffer, bufferLen);
		m_isSendData = true; // 제일 마지막에 세팅해야 한다.
		return bufferLen;
	}

	return 0;
}


void cSerialAsync::Close()
{
	m_isConnect = false;
	m_threadLoop = false;
	if (m_handle)
	{
		::WaitForSingleObject(m_handle, 1000);
		m_handle = NULL;
	}
}


// UDP 서버 쓰레드
unsigned WINAPI SerialThreadFunction(void* arg)
{
	cSerialAsync *serial = (cSerialAsync*)arg;

	char buffer[cSerialAsync::BUFLEN];
	int bufferLen = 0;
	while (serial->m_threadLoop && serial->m_isConnect)
	{
		if (serial->m_isSendData)
		{
			cAutoCS cs(serial->m_CriticalSection);
			bufferLen = serial->m_bufferLen;
			memcpy(buffer, serial->m_buffer, serial->m_bufferLen);
			serial->m_isSendData = false;
		}
		else
		{
			bufferLen = 0;
		}

		if (bufferLen > 0)
			serial->m_serial.SendData(buffer, bufferLen);

		Sleep(serial->m_sleepMillis);
	}

	serial->m_serial.Close();

	return 0;
}
