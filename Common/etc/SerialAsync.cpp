
#include "stdafx.h"
#include "SerialAsync.h"

using namespace common;


cSerialAsync::cSerialAsync()
	: m_isConnect(false)
	, m_isSendData(false)
	, m_sleepMillis(10)
	, m_threadLoop(true)
{
}

cSerialAsync::~cSerialAsync()
{
	Close();
}


bool cSerialAsync::Open(const int portNum, const int baudRate)
{
	RETV(m_serial.IsOpened(), true);
	if (!m_serial.Open(portNum, baudRate))
		return false;
	
	m_isConnect = true;
	m_threadLoop = true;
	m_thread = std::thread(SerialThreadFunction, this);

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
		AutoCSLock cs(m_cs);
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
	if (m_thread.joinable())
		m_thread.join();
}


unsigned cSerialAsync::SerialThreadFunction(cSerialAsync * serial)
{
	char buffer[cSerialAsync::BUFLEN];
	int bufferLen = 0;
	while (serial->m_threadLoop && serial->m_isConnect)
	{
		if (serial->m_isSendData)
		{
			AutoCSLock cs(serial->m_cs);
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
