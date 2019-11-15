
#include "stdafx.h"
#include "SerialAsync.h"

using namespace common;


cSerialAsync::cSerialAsync()
	: m_state(eState::DISCONNECT)
	, m_sleepMillis(10)
	, m_delimeter('\n')
{
}

cSerialAsync::~cSerialAsync()
{
	Close();
}


bool cSerialAsync::Open(const int portNum, const int baudRate
	, const char delimeter //= '\n'
)
{
	Close();

	RETV(m_serial.IsOpened(), true);
	if (!m_serial.Open(portNum, baudRate))
		return false;
	
	m_state = eState::CONNECT;
	m_delimeter = delimeter;
	m_thread = std::thread(SerialThreadFunction, this);

	return true;
}


bool cSerialAsync::IsOpen() const
{
	return (m_state == eState::CONNECT) && m_serial.IsOpened();
}


int cSerialAsync::SendData(BYTE *buffer, const int size)
{
	if (m_sndQ.full())
		return 0;

	{
		AutoCSLock cs(m_cs);

		char buf[2];
		*(u_short*)buf = (u_short)size;
		m_sndQ.push(buf, 2); // store buffer length
		m_sndQ.push((char*)buffer, size); // store buffer data
	}
	return size;
}


// delimeter 까지 읽은 정보를 하나씩 리턴한다.
// 받은 데이타가 없다면 0을 리턴한다.
uint cSerialAsync::RecvData(BYTE *buffer, const int size)
{
	if (m_rcvQ.empty())
		return 0;

	AutoCSLock cs(m_cs);

	// read data length
	char lbuff[2];
	if (!m_rcvQ.pop(lbuff, 2))
		return 0; // error occurred!

	u_short readLen = *(u_short*)lbuff;
	if (size < (int)readLen)
		return 0; // error occurred!

	// read data
	if (!m_rcvQ.pop((char*)buffer, readLen))
		return 0; // error occurred!
	return readLen;
}


void cSerialAsync::SetDelimeter(const char delimeter)
{
	m_delimeter = delimeter;
}


void cSerialAsync::Close()
{
	m_state = eState::DISCONNECT;
	if (m_thread.joinable())
		m_thread.join();
	m_serial.Close();
	m_sndQ.clear();
	m_rcvQ.clear();
}


unsigned cSerialAsync::SerialThreadFunction(cSerialAsync * asyncSer)
{
	char buffer[cSerialAsync::BUFLEN];
	while (asyncSer->m_state == eState::CONNECT)
	{
		// 1. Send Process
		int sndLen = 0;
		if (!asyncSer->m_sndQ.empty())
		{
			AutoCSLock cs(asyncSer->m_cs);

			// read data length
			char lbuff[2];
			if (asyncSer->m_sndQ.pop(lbuff, 2))
			{
				u_short readLen = *(u_short*)lbuff;
				if (sizeof(buffer) > (uint)readLen)
				{
					// read data
					if (asyncSer->m_sndQ.pop((char*)buffer, readLen))
					{
						sndLen = readLen;
					}
				}
			}
		}

		if (sndLen > 0)
			asyncSer->m_serial.SendData(buffer, sndLen);

		// 2. Receive Process
		int rcvLen = 0;
		asyncSer->m_serial.ReadStringUntil(asyncSer->m_delimeter, buffer, rcvLen, sizeof(buffer));
		if (rcvLen > 0)
		{
			AutoCSLock cs(asyncSer->m_cs);
			
			// 2 byte read len (ushort)
			// n byte read data
			if (asyncSer->m_rcvQ.size() < (uint)(2 + rcvLen))
			{
				char buf[2];
				*(u_short*)buf = (u_short)rcvLen;
				asyncSer->m_rcvQ.push(buf, 2); // store buffer length
				asyncSer->m_rcvQ.push(buffer, rcvLen); // store buffer data
			}
			else
			{
				// error occurred!!, queue full
			}
		}

		Sleep(asyncSer->m_sleepMillis);
	}

	return 0;
}
