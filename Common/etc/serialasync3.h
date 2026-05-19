//
// 2025-02-17, jjuiddong
// async serial class
//	- dynamic length serial communication
// 
// 2025-07-28
//	- refactoring read serial process
//
// serial communication protocol
// packet format
//	- header: 0xAA, 1 byte
//	- length : data length, 1 byte
//	- data : packet data, length byte size - 1
//	- parity bit : checksum xor bit, 1 byte
//
#pragma once


namespace common
{
	class cSerialAsync3
	{
	public:
		cSerialAsync3();
		virtual ~cSerialAsync3();

		bool Init(const int port, const int baud = -1);
		int Read(uchar* buff, const uint size);
		bool Send(const uchar* buff, const uint size);
		bool Flush();
		bool IsOpen() const;
		bool IsConnect() const;
		bool Close();


	protected:
		bool ReadSerial();
		bool ParseBuffer();
		bool ParseProtocol();
		bool MakePacket(uchar* buff, const int size
			, OUT uchar* data, OUT int* dataSize);
		int BufferSize();
		char PopBuffer();
		bool MoveBuffer(const int size);
		bool IsEmpty();
		void ClearBuffer();

		static int SerialThreadFunction(cSerialAsync3* ser);


	public:
		enum { 
			PACKET_SIZE = 64, // max packet size
			BUFFER_SIZE = 512, // send/recv temporal buffer size
			QUEUE_SIZE = 1024,// send/recv queue size
		};
		enum class eState { Disconnect, TryConnect, Connect };

		eState m_state;
		cSerial2 m_serial; // serial communication object
		cCircularQueue2<uchar> m_sndQ; // send queue
		cCircularQueue2<uchar> m_rcvQ; // recv queue
		bool m_isFlushCmd; // flush command?
		int m_errCnt1; // parity bit error count for debug

		// receive circular buffer
		uchar m_rcvBuff[BUFFER_SIZE];
		int m_front; // front index, receive circular buffer
		int m_rear; // rear index, receive circular buffer
		int m_length; // packet byte size, receive circular buffer
		int m_readState; // read state, receive circular buffer

		// thread
		std::thread m_thread;
		CriticalSection m_wcs; // write cs
		CriticalSection m_rcs; // read cs
	};
}
