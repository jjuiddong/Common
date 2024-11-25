
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
	, m_packBytes(2)
	, m_readBytes(6)
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
bool cSerialAsync2::Open(const int portNum, const int baudRate
	, const int readBytes //=6
)
{
	if (eState::Connect == m_state)
		return true; // already open

	Close();

	if (!m_serial.Open(portNum, baudRate))
		return false;

	m_port = portNum;
	m_baudRate = baudRate;
	m_readBytes = readBytes;
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
		*(ushort*)buf = size + m_packBytes;
		m_sndQ.push(buf, 2);

		buf[0] = (char)m_sndProtocol.startBit;
		m_sndQ.push(buf, 1);
		m_sndQ.push((char*)buffer, size);

		if ((4 == m_rcvProtocol.endBitCnt) && (4 == m_rcvProtocol.parityCnt))
		{
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
		else
		{
			// parity bit (8bit)
			uint parity = 0;
			for (uint i = 0; i < size; ++i)
				parity = buffer[i] ^ parity;
			// parity bit, end bit
			buf[0] = parity;
			buf[1] = m_sndProtocol.endBit;
			m_sndQ.push(buf, 2);
		}		
	}
	return size;
}


// read data
// buffer: recv data buffer
// size: recv buffer size
// return: data bytes size
//		  -1: connect error
//		  -2: internal error
//		  -3: start bit error
//		  -4: end bit error
//		  -5: parity bit error
int cSerialAsync2::RecvData(BYTE *buffer, const uint size)
{
	if (eState::Connect != m_state)
		return -1; // not connected
	if (m_rcvQ.empty())
		return 0;
	if (0 == size)
		return 0; // error return

	{
		AutoCSLock cs(m_cs);

		// read data length
		char buff[2];
		if (!m_rcvQ.pop(buff, 2))
			return -2; // error occurred!

		const ushort readLen = *(ushort*)buff;
		if (size < (int)readLen)
			return -2; // error occurred!

		// read data
		char tmpBuffer[BUFLEN];
		if (!m_rcvQ.pop((char*)tmpBuffer, readLen))
			return -2; // error return

		// check start bit, end bit, parity bit
		// 4: bytes size (start bit + end bit + parity bit + data bit)
		if (readLen != m_readBytes)
			return -2; // error return, must 4 byte

		// check start bit
		if ((tmpBuffer[0] & 0xFF) != m_rcvProtocol.startBit)
			return -3; // error return

		if ((4 == m_rcvProtocol.endBitCnt) && (4 == m_rcvProtocol.parityCnt))
		{
			// check end bit
			if ((int)(tmpBuffer[readLen - 1] & 0x0F) != m_rcvProtocol.endBit)
				return -4; // error return, end bit error

			// check parity bit
			uint parity = 0; // parity bit (4bit)
			for (ushort i = 1; i < readLen - 1; ++i)
			{
				parity = (tmpBuffer[i] & 0x0F) ^ parity;
				parity = ((tmpBuffer[i] & 0xF0) >> 4) ^ parity;
			}
			if (((int)(tmpBuffer[readLen - 1] & 0xF0) >> 4) != parity)
				return -5; // error return, parity bit error
		}
		else
		{
			// check end bit
			if ((tmpBuffer[readLen - 1] & 0xFF) != m_rcvProtocol.endBit)
				return -4; // error return, end bit error

			// check parity bit
			uint parity = 0; // parity bit (8bit)
			for (ushort i = 1; i < readLen - 2; ++i)
				parity = tmpBuffer[i] ^ parity;
			if ((tmpBuffer[readLen - 2] & 0xFF) != (parity & 0xFF))
			{
				dbg::Logc(1, "error parity bit, src:0x%x, calc:0x%x\n"
					, tmpBuffer[readLen - 2], parity);
				return -5; // error return, parity bit error
			}
		}

		for (ushort i = 1; i < (readLen - 1); ++i)
			buffer[i - 1] = tmpBuffer[i];

		return readLen - m_packBytes; // data size
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
				const int rcvLen = ser->m_serial.ReadData(rcvBuffer, ser->m_readBytes);
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
					if (remainSize > (uint)(ser->m_packBytes + rcvLen))
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
