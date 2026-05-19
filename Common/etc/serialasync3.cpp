
#include "stdafx.h"
#include "serialasync3.h"

using namespace std;
using namespace common;


cSerialAsync3::cSerialAsync3()
	: m_state(eState::Disconnect)
	, m_sndQ(QUEUE_SIZE)
	, m_rcvQ(QUEUE_SIZE)
	, m_errCnt1(0)
	, m_isFlushCmd(false)
	, m_front(0)
	, m_rear(0)
	, m_readState(0)
	, m_length(0)
{
}

cSerialAsync3::~cSerialAsync3()
{
	Close();
}


// initialize async serial
bool cSerialAsync3::Init(const int port
	, const int baud //= -1
)
{
	Close();

	if (!m_serial.Open(port, baud))
		return false;

	//m_serial.Flush();
	m_state = eState::Connect;
	m_thread = std::thread(SerialThreadFunction, this);
	return true;
}


// read serial data from async buffer
// return read byte size
int cSerialAsync3::Read(uchar* buff, const uint size)
{
	RETV(m_rcvQ.empty(), 0);
	{
		AutoCSLock cs(m_rcs);

		// read data length
		uchar header[2] = {0, 0};
		int len = 0;
		if (m_rcvQ.pop(header, 2))
		{
			const ushort readLen = *(ushort*)header;
			if (size > (uint)readLen)
			{
				// read data from queue
				if (m_rcvQ.pop(buff, readLen))
					len = readLen;
			}
		}
		else
		{
			m_rcvQ.clear(); // excecption process
		}
		return len;
	}
	return 0;
}


// write serial data to async buffer
bool cSerialAsync3::Send(const uchar* buff, const uint size)
{
	RETV(0 == size, false);
	if (!IsConnect())
	{
		std::cout << "error cSerialAsync3.Send, disconnect state" << std::endl;
		return false;
	}

	{
		AutoCSLock cs(m_wcs);
		uchar header[2] = { 0, 0 };
		*(ushort*)header = (ushort)size;
		m_sndQ.push(header, 2);
		m_sndQ.push(buff, size);
	}
	return true;
}


// flush serial
bool cSerialAsync3::Flush()
{
	m_isFlushCmd = true;
	return true;
}


// is open serial?
bool cSerialAsync3::IsOpen() const
{
	return m_serial.IsOpen();
}


// is connect serial?
bool cSerialAsync3::IsConnect() const
{
	return m_state != eState::Disconnect;
}


// read serial buffer
// return: success read?
bool cSerialAsync3::ReadSerial()
{
	int numByteRcv = 0;
	//m_serial.NumberByteRcv(numByteRcv);
	if (numByteRcv <= 0)
		return true; // nothing to do

	const int remainSize = (m_front <= m_rear) ? (BUFFER_SIZE - m_rear) : (m_front - m_rear - 1);
	if (remainSize > 0)
	{
		const int minByteLen = min(remainSize, numByteRcv);
		const int size = m_serial.ReadData(m_rcvBuff + m_rear, minByteLen);
		if (size < 0)
		{
			m_state = eState::Disconnect;
			m_serial.Close();
			cout << "terminate serial (2)" << endl;
			return false; // fail
		}
		if (size > 0)
		{
			m_rear = (m_rear + size) % BUFFER_SIZE;

			// if remain another data, read more
			if ((size == remainSize) && (remainSize < numByteRcv))
			{
				const int remainSize2 = (m_front <= m_rear) ? (BUFFER_SIZE - m_rear) : (m_front - m_rear - 1);
				const int minByteLen2 = min(remainSize2, numByteRcv - size);
				if (minByteLen2 > 0)
				{
					const int size2 = m_serial.ReadData(m_rcvBuff + m_rear, minByteLen2);
					if (size2 > 0)
						m_rear = (m_rear + size2) % BUFFER_SIZE;
				}
			}
			//~
		}
	}
	else
	{
		// exception process: clear buffer
		m_rear = m_front = 0;
	}

	return true;
}


// parse receive serial buffer
bool cSerialAsync3::ParseBuffer()
{
	while (m_front != m_rear)
	{
		switch (m_readState)
		{
		case 0: // read start code
		{
			const unsigned char c = PopBuffer();
			if (c == 0xAA)
			{
				m_readState = 1; // goto next step
			}
		}
		break;

		case 1: // read packet length
		{
			const unsigned char c = PopBuffer();
			if (c > PACKET_SIZE)
			{
				m_readState = 0;
				continue; // error, retry
			}
			m_readState = 2; // goto next step
			m_length = (unsigned int)c;
		}
		break;

		case 2: // read data
			if (BufferSize() >= m_length)
			{
				m_readState = 0;
				const int front = m_front;
				const bool result = ParseProtocol();

				// check all data read
				const int readSize = (m_front > front) ? (m_front - front) : (BUFFER_SIZE - front + m_front);
				if (readSize < m_length)
				{
					const int count = m_length - readSize;
					MoveBuffer(count);
				}
				//~
				return result; // finish
			}
			else
			{
				return false; // read more data
			}
			break;

		default: m_readState = 0; return false; // exception
		}//~switch
	}//~while

	return true;
}


// parse protocol
// return: success parse?
bool cSerialAsync3::ParseProtocol()
{
	// check parity bit
	{
		int parity = 0;
		int size = 0;
		int i = m_front;
		while (size++ < (m_length - 1))
		{
			parity = m_rcvBuff[i] ^ parity;
			i = (i + 1) % BUFFER_SIZE;
		}
		if ((m_rcvBuff[i] & 0xFF) != parity)
		{
			++m_errCnt1;
			if (m_errCnt1 > 255)
				m_errCnt1 = 0;
			return false;  // error return
		}
	}
	//~

	// copy buffer except parity bit
	uchar outBuff[BUFFER_SIZE];
	for (int i = 0; i < m_length - 1; ++i)
		outBuff[i] = PopBuffer();
	PopBuffer(); // pop parity bit

	{
		AutoCSLock cs(m_rcs);
		// 2 byte read len (ushort)
		// n byte read data
		const uint dataSize = (uint)(m_length - 1);
		const uint remainSize = m_rcvQ.SIZE - m_rcvQ.size();
		if (remainSize > dataSize)
		{
			uchar buff[2];
			*(ushort*)buff = (ushort)dataSize;
			m_rcvQ.push(buff, 2); // store buffer length
			m_rcvQ.push(outBuff, dataSize); // store buffer data
		}
		else
		{
			// error occurred!!, queue full
		}
	}

	return true;
}


// make packet
// buff, size: source packet data
// data, dataSize: return packet with header, checksum
bool cSerialAsync3::MakePacket(uchar* buff, const int size
	, OUT uchar* data, OUT int* dataSize)
{
	data[0] = 0xAA;
	data[1] = (uchar)size + 1; // 1: parity bit
	for (int i = 0; i < size; ++i)
		data[i + 2] = buff[i];

	// make parity bit
	int parity = 0;
	for (int i = 0; i < size; ++i)
		parity = buff[i] ^ parity;
	data[size + 2] = (uchar)(parity & 0xFF);

	*dataSize = size + 3; // 3: header + parity bit
	return true;
}



//-----------------------------------------------------------------------------
// read buffer size
int cSerialAsync3::BufferSize()
{
	if (m_front <= m_rear)
		return m_rear - m_front;
	return (BUFFER_SIZE - m_front) + m_rear;
}


//-----------------------------------------------------------------------------
// read buffer 1 byte
char cSerialAsync3::PopBuffer()
{
	const char c = m_rcvBuff[m_front];
	m_front = (m_front + 1) % BUFFER_SIZE;
	return c;
}


//-----------------------------------------------------------------------------
// move front index
bool cSerialAsync3::MoveBuffer(const int size)
{
	const int moveSize = std::min(BufferSize(), size);
	if (0 >= moveSize)
		return false;
	m_front = (m_front + moveSize) % BUFFER_SIZE;
	return true;
}


//-----------------------------------------------------------------------------
// is buffer empty?
bool cSerialAsync3::IsEmpty()
{
	return m_front == m_rear;
}


//-----------------------------------------------------------------------------
// clear buffer
void cSerialAsync3::ClearBuffer()
{
	m_front = 0;
	m_rear = 0;
}


// close serial
bool cSerialAsync3::Close()
{
	m_state = eState::Disconnect;
	if (m_thread.joinable())
		m_thread.join();
	m_serial.Close();
	m_sndQ.clear();
	m_rcvQ.clear();
	return true;
}


// serial thread function
int cSerialAsync3::SerialThreadFunction(cSerialAsync3* ser)
{
	if (ser->m_state != eState::Connect)
		return 0;

	uchar sndBuff[BUFFER_SIZE];
	int readIdx = 0; // rcvBuff read index
	//uchar rcvBuff[BUFFER_SIZE]; // serial receive circular buffer
	int front = 0; // circular buffer front index
	int rear = 0; // circular buffer rear index

	while (eState::Connect == ser->m_state)
	{
		if (ser->m_isFlushCmd)
		{
			ser->m_isFlushCmd = false; // clear
			ser->Flush();
		}

		// 1. Send Process
		if (!ser->m_sndQ.empty())
		{
			AutoCSLock cs(ser->m_wcs);

			uchar buff[2] = { 0, 0 };
			while (!ser->m_sndQ.empty())
			{
				// read data length
				int len = 0;
				if (ser->m_sndQ.pop(buff, 2))
				{
					const ushort readLen = *(ushort*)buff;
					if (sizeof(sndBuff) > (uint)readLen)
					{
						// read data from queue
						if (ser->m_sndQ.pop(sndBuff, readLen))
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

				if (len <= 0)
					break;

				int packetSize = 0;
				uchar outBuff[BUFFER_SIZE];
				ser->MakePacket(sndBuff, len, outBuff, &packetSize);
				if (0 == ser->m_serial.SendData((const char*)outBuff, packetSize))
				{
					ser->m_state = eState::Disconnect;
					ser->m_serial.Close();
					cout << "terminate serial (1)" << endl;
					return 0; // fail
				}
				break; // finish, send only one
			}//~while
		}

		// 2. Receive Process
		ser->ReadSerial();
		ser->ParseBuffer();

		std::this_thread::sleep_for(std::chrono::milliseconds(0)); // thread context swithing
	}//~while

	return 1;
}
