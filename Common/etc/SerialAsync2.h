//
// 2024-04-12, jjuiddong
//	async serial communication
//
//	serial protocol
//	start, data, parity bit, end
//		- start bit: '@' (8 bit)
//		- end bit: 00 (2 bit)
//		- parity bit: xor (2 bit)
// 
#pragma once


namespace common
{

	class cSerialAsync2
	{
	public:
		cSerialAsync2();
		virtual ~cSerialAsync2();

		bool Open(const int portNum, const int baudRate, const int readBytes = 6);
		int SendData(BYTE *buffer, const uint size);
		int RecvData(BYTE *buffer, const uint size);
		bool IsOpen() const;
		void Close();


	public:
		static int SerialThreadFunction(cSerialAsync2 *asyncSer);


	public:
		enum { BUFLEN = 512, MAX_BUFFERSIZE = 1024 };
		enum class eState { Disconnect, TryConnect, Connect};

		// serial data protocol format information
		struct sProtocol
		{
			uint startBit; // start bit data
			uint startBitCnt; // start bit count
			uint endBit; // end bit data
			uint endBitCnt; // end bit count
			uint parityCnt; // parity bit count
			uint dataCnt; // data bit count
		};

		eState m_state;
		cSerial2 m_serial;
		int m_port; // port number
		int m_baudRate; // baud rate
		int m_packBytes; // packing byte size (start bit + end bit + parity bit, default:2)
		int m_readBytes; // read serial data byte size (default:6)
		cCircularQueue2<char> m_sndQ; // send queue
		cCircularQueue2<char> m_rcvQ; // receive queue
		sProtocol m_sndProtocol; // send protocol format
		sProtocol m_rcvProtocol; // recv protocol format
		int m_sleepMillis; // default = 10ms
		std::thread m_thread;
		CriticalSection m_cs;
	};

}
