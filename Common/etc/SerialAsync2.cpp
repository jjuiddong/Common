
#include "stdafx.h"
#include "SerialAsync2.h"

using namespace common;


cSerialAsync2::cSerialAsync2()
	: m_state(eState::Disconnect)
	, m_port(0)
	, m_baudRate(0)
	, m_sndQ(MAX_BUFFERSIZE)
	, m_rcvQ(MAX_BUFFERSIZE)
	, m_sleepMillis(10)
{
	m_sndProtocol.startBit = (uint)'@';
	m_sndProtocol.startBitCnt = 8;
	m_sndProtocol.endBit = 0x0;
	m_sndProtocol.endBitCnt = 4;
	m_sndProtocol.parityCnt = 4;
	m_sndProtocol.dataCnt = 24;

	m_rcvProtocol.startBit = (uint)'#';
	m_rcvProtocol.startBitCnt = 8;
	m_rcvProtocol.endBit = 0xF; // 1111b
	m_rcvProtocol.endBitCnt = 4;
	m_rcvProtocol.parityCnt = 4;
	m_sndProtocol.dataCnt = 8;
}

cSerialAsync2::~cSerialAsync2()
{
	Close();
}


// open serial
bool cSerialAsync2::Open(const int portNum, const int baudRate)
{
	if (eState::Connect == m_state)
		return true; // already open

	Close();

	if (!m_serial.Open(portNum, baudRate))
		return false;

	m_port = portNum;
	m_baudRate = baudRate;
	m_state = eState::Connect;
	m_thread = std::thread(SerialThreadFunction, this);
	return true;
}


// serial open?
bool cSerialAsync2::IsOpen() const
{
	return (eState::Connect == m_state) && m_serial.IsOpen();
}


// send data
int cSerialAsync2::SendData(BYTE *buffer, const uint size)
{
	if (eState::Connect != m_state)
		return 0;
	if (m_sndQ.full())
		return 0;
	if (0 == size)
		return 0; // nothing to do

	{
		AutoCSLock cs(m_cs);

		char buf[2];
		*(ushort*)buf = size + 2; // 2: start bit + end bit + pairty bit (2byte)
		m_sndQ.push(buf, 2);

		buf[0] = (char)m_sndProtocol.startBit;
		m_sndQ.push(buf, 1);
		m_sndQ.push((char*)buffer, size);

		// parity bit (4bit)
		uint parity = 0;
		for (uint i = 0; i < size; ++i)
		{
			parity = (buffer[i] & 0x0F) ^ parity;
			parity = ((buffer[i] & 0xF0) >> 4) ^ parity;
		}
		// parity bit + end bit
		buf[0] = (char)((parity << 4) | m_sndProtocol.endBit);
		m_sndQ.push(buf, 1);
	}
	return size;
}


// read data
// buffer: recv data buffer
// size: recv buffer size
// return: data bytes size
uint cSerialAsync2::RecvData(BYTE *buffer, const uint size)
{
	if (eState::Connect != m_state)
		return 0;
	if (m_rcvQ.empty())
		return 0;
	if (0 == size)
		return 0; // error return

	{
		AutoCSLock cs(m_cs);

		// read data length
		char buff[2];
		if (!m_rcvQ.pop(buff, 2))
			return 0; // error occurred!

		const ushort readLen = *(ushort*)buff;
		if (size < (int)readLen)
			return 0; // error occurred!

		// read data
		char tmpBuffer[BUFLEN];
		if (!m_rcvQ.pop((char*)tmpBuffer, readLen))
			return 0; // error return

		// check start bit, end bit, parity bit
		// 3: bytes size (start bit + end bit + parity bit + data bit)
		if (readLen != 3)
			return 0; // error return

		// check start bit
		if ((int)tmpBuffer[0] != m_rcvProtocol.startBit)
			return 0; // error return

		// check end bit
		if ((int)(tmpBuffer[2] & 0x0F) != m_rcvProtocol.endBit)
			return 0; // error return

		// check parity bit
		uint parity = 0; // parity bit (4bit)
		for (uint i = 0; i < 1; ++i)
		{
			parity = (tmpBuffer[i + 1] & 0x0F) ^ parity;
			parity = ((tmpBuffer[i + 1] & 0xF0) >> 4) ^ parity;
		}
		if (((int)(tmpBuffer[2] & 0xF0) >> 4) != parity)
			return 0; // error return

		buffer[0] = tmpBuffer[1];

		return 1; // data size 1 byte
	}

	return 0;
}


// close serial
void cSerialAsync2::Close()
{
	m_state = eState::Disconnect;
	if (m_thread.joinable())
		m_thread.join();
	m_serial.Close();
	m_sndQ.clear();
	m_rcvQ.clear();
}


// multithread function, serial read/write
int cSerialAsync2::SerialThreadFunction(cSerialAsync2* ser)
{
	if (ser->m_state != eState::Connect)
		return 0;

	//if (cSerialAsync2::eState::TryConnect != ser->m_state)
	//	return 0;
	//if (!ser->m_serial.Open(ser->m_port, ser->m_baudRate))
	//{
	//	ser->m_state = eState::Disconnect;
	//	return 0;
	//}
	//ser->m_state = cSerialAsync2::eState::Connect;

	char sndBuffer[cSerialAsync2::BUFLEN];
	char rcvBuffer[cSerialAsync2::BUFLEN];
	while (eState::Connect == ser->m_state)
	{
		// 1. Send Process
		{
			AutoCSLock cs(ser->m_cs);

			char buff[2];
			while (!ser->m_sndQ.empty())
			{
				// read data length
				int len = 0;
				if (ser->m_sndQ.pop(buff, 2))
				{
					const ushort readLen = *(ushort*)buff;
					if (sizeof(sndBuffer) > (uint)readLen)
					{
						// read data from queue
						if (ser->m_sndQ.pop(sndBuffer, readLen))
							len = readLen;
					}
					else
					{
						ser->m_sndQ.clear(); // excecption process
					}
				}
				else
				{
					ser->m_sndQ.clear(); // excecption process
				}

				if (0 == len)
					break;
				if (0 == ser->m_serial.SendData(sndBuffer, len))
				{
					ser->m_state = eState::Disconnect;
					ser->m_serial.Close();
					return 0; // fail
				}
			}//~while
		}

		// 2. Receive Process
		{
			int cnt = 0; // prevent infinity loop
			while (cnt++ < 10)
			{
				const int rcvLen = ser->m_serial.ReadData(rcvBuffer, 3);
				if (0 == rcvLen)
					break;

				if (rcvLen < 0)
				{
					ser->m_state = eState::Disconnect;
					ser->m_serial.Close();
					return 0; // fail
				}

				if (rcvLen > 0)
				{
					AutoCSLock cs(ser->m_cs);
				
					// 2 byte read len (ushort)
					// n byte read data
					const uint remainSize = ser->m_rcvQ.SIZE - ser->m_rcvQ.size();
					if (remainSize > (uint)(2 + rcvLen))
					{
						char buff[2];
						*(ushort*)buff = (ushort)rcvLen;
						ser->m_rcvQ.push(buff, 2); // store buffer length
						ser->m_rcvQ.push(rcvBuffer, rcvLen); // store buffer data
					}
					else
					{
						// error occurred!!, queue full
						break;
					}
				}
			}//~while
		}

		Sleep(ser->m_sleepMillis);
	}

	return 0;
}
