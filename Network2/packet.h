//
// 2019-01-08, jjuiddong
// Packet class
//
// definition Protocol Header by iPacketHeader
//
#pragma once


namespace network2
{
	class cPacket
	{
	public:
		cPacket();
		cPacket(iPacketHeader *packetHeader);
		cPacket(iPacketHeader *packetHeader, const BYTE *src, const int byteSize);
		cPacket(const cPacket &rhs);
		virtual ~cPacket();

		// call before read/write
		void InitRead(); 

		// call before send packet
		void EndPack();

		template<class T> void Append(const T &rhs);
		template<class T> void AppendPtr(const T &rhs, const size_t size);
		template<class T> void GetData(OUT T &rhs);
		template<class T> void GetDataPtr(OUT T &rhs, size_t size);
		void AddDelimeter();
		void AppendDelimeter(const char c);
		void GetDataString(OUT string &str);
		int GetDataString(const char delimeter1, const char delimeter2, OUT string &str);
		int GetDataAscii(const char delimeter1, const char delimeter2, OUT char *buff, const int buffLen);

		void SetProtocolId(const int protocolId);
		void SetPacketId(const int packetId);
		void SetPacketSize(const short packetSize);
		void SetPacketOption(const uint mask, const uint option);
		void SetSenderId(const netid netId);
		int GetProtocolId() const;
		uint GetPacketId() const;
		uint GetPacketSize() const;
		uint GetPacketOption(const uint mask);
		int GetWriteSize() const;
		int GetSenderId() const;
		int GetHeaderSize() const;
		bool IsValid();

		cPacket& operator=(const cPacket &rhs);


	public:
		iPacketHeader *m_packetHeader; // reference
		netid m_sndId;
		int m_readIdx;
		int m_writeIdx;
		char m_lastDelim; // GetDataString, GetDataAscii, last delimeter
		bool m_emptyData; // GetDataAscii
		BYTE m_data[DEFAULT_PACKETSIZE];
	};


	//--------------------------------------------------------------------------------------
	// Implements

	// m_writeIdx 부터 데이타를 저장한다.
	template<class T>
	inline void cPacket::Append(const T &rhs)
	{
		if (m_writeIdx + sizeof(T) >= DEFAULT_PACKETSIZE)
			return;
		memmove_s(m_data + m_writeIdx, DEFAULT_PACKETSIZE - m_writeIdx, &rhs, sizeof(T));
		m_writeIdx += sizeof(T);
	}

	// size : copy byte size
	template<class T>
	inline void cPacket::AppendPtr(const T &rhs, const size_t size)
	{
		if (m_writeIdx + size >= DEFAULT_PACKETSIZE)
			return;
		memmove_s(m_data + m_writeIdx, DEFAULT_PACKETSIZE - m_writeIdx, rhs, size);
		m_writeIdx += size;
	}

	// delimeter를 추가한다.
	inline void cPacket::AddDelimeter()
	{
		if (m_writeIdx + 1 >= DEFAULT_PACKETSIZE)
			return;
		const int len = m_packetHeader->SetDelimeter(&m_data[m_writeIdx]);
		m_writeIdx += len;
	}

	// delimeter를 추가한다.
	inline void cPacket::AppendDelimeter(const char c)
	{
		if (m_writeIdx + 1 >= DEFAULT_PACKETSIZE)
			return;
		m_data[m_writeIdx++] = c;
	}

	// m_readIdx 부터 데이타를 가져온다.
	template<class T>
	inline void cPacket::GetData(OUT T &rhs)
	{
		if (m_readIdx + sizeof(T) >= DEFAULT_PACKETSIZE)
			return;
		memmove_s(&rhs, sizeof(T), m_data + m_readIdx, sizeof(T));
		m_readIdx += sizeof(T);
	}

	template<class T>
	inline void cPacket::GetDataPtr(OUT T &rhs, size_t size)
	{
		if (m_readIdx + size >= DEFAULT_PACKETSIZE)
			return;
		memmove_s(rhs, size, m_data + m_readIdx, size);
		m_readIdx += size;
	}

	// NULL 문자가 나올때 까지 복사한 후 리턴한다.
	inline void cPacket::GetDataString(OUT string &str)
	{
		char buf[DEFAULT_PACKETSIZE] = { NULL, };
		for (int i = 0; i < DEFAULT_PACKETSIZE - 1 && (m_readIdx < DEFAULT_PACKETSIZE); ++i)
		{
			buf[i] = m_data[m_readIdx++];
			if (NULL == m_data[m_readIdx - 1])
				break;
		}
		str = buf;
	}

	// delimeter가 나올 때까지 ascii를 복사해서 리턴한다.
	// 문자열이 쌍따옴표로 시작했다면, 쌍따옴표로 끝날때 까지 읽는다.
	// 이때 delimeter는 무시된다.
	inline int cPacket::GetDataString(const char delimeter1, const char delimeter2, OUT string &str)
	{
		int i = 0;
		char c = NULL;
		bool isStart = true;
		bool isDoubleQuote = false;
		char buff[DEFAULT_PACKETSIZE] = { NULL, };
		while ((m_readIdx < DEFAULT_PACKETSIZE) && (i < (DEFAULT_PACKETSIZE - 1)))
		{
			c = m_data[m_readIdx++];
			if (isStart && (c == '\"'))
			{
				isStart = false;
				isDoubleQuote = true;
				continue;
			}

			if (isDoubleQuote && ((c == '\"') || (c == NULL)))
				break;
			if (!isDoubleQuote && ((c == delimeter1) || (c == delimeter2) || (c == NULL)))
				break;

			buff[i++] = c;
			isStart = false;
		}

		if (i < DEFAULT_PACKETSIZE)
			buff[i] = NULL;
		str = buff;
		m_lastDelim = c;
		m_emptyData = (i <= 0);
		return i;
	}


	// delimeter가 나올 때까지 ascii를 복사해서 리턴한다.
	inline int cPacket::GetDataAscii(const char delimeter1, const char delimeter2
		, OUT char *buff, const int buffLen)
	{
		int i = 0;
		char c = NULL;
		while ( (m_readIdx < DEFAULT_PACKETSIZE) && (i < (buffLen-1)))
		{
			c = m_data[m_readIdx++];
			if ((c == delimeter1) || (c == delimeter2) || (c == NULL))
				break;
			buff[i++] = c;
		}
		if (i < buffLen)
			buff[i] = NULL;
		m_lastDelim = c;
		m_emptyData = (i <= 0);
		return i;
	}


	//--------------------------------------------------------------------------------------
	// Global Reserved Packet ID
	enum RESERVED_PACKETID
	{
		PACKETID_CONNECT = 1,
		PACKETID_DISCONNECT,
		PACKETID_CLIENT_DISCONNECT,
		PACKETID_ACCEPT,
	};

	class cNetworkNode;
	cPacket DisconnectPacket(cNetworkNode *node, netid disconnectId);
	cPacket ClientDisconnectPacket(cNetworkNode *node, netid disconnectId);
	cPacket AcceptPacket(cNetworkNode *node, SOCKET acceptSocket, const string &clientIP, int port);
	//--------------------------------------------------------------------------------------
}
