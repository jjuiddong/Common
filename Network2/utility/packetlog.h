//
// 2019-03-09, jjuiddong
// Packet Log Reader
//	- *.plog file read
//	- write network2::LogPacket()
//
#pragma once


namespace network2
{

	struct sPacketLogData
	{
		int type; // 0:session, 1:packet
		netid sndId; // type=1, sender id
		netid rcvId; // type=1, receiver id
		uint size; // type=1, data size
		BYTE data[DEFAULT_PACKETSIZE];
	};


	class cPacketLog
	{
	public:
		struct sPacketLogInfo : public common::cMemoryPool4<sPacketLogInfo>
		{
			uint64 dateTime; // yyyymmddhhmmssmmm
			netid sndId;
			netid rcvId;
			uint size;
			cPacket *packet;
			StrId name; // packet name
			hashcode nameHashcode; // for fast compare
			string packetInfo;
		};

		cPacketLog();
		virtual ~cPacketLog();

		bool Read(const char *fileName);
		bool ReadAll(const char *fileName);
		uint Streaming();
		sPacketLogInfo* ReadPacket(const uint64 dateTime);
		cDateTimeRange GetTimeRange(const char *fileName);
		void Clear();

		static bool Write(const char *fileName, const sPacketLogData &logData);


	protected:
		uint ReadStream(std::istream &ifs);
		std::streampos GetReadPos(const char *fileName);


	public:
		netid m_id; // session id
		StrId m_name; // session Name
		int m_totalCount; // total packet count
		bool m_isCompareDisplay; // dateTime compare display on/off
		cPacketHeader m_binPacketHeader;
		cPacketHeaderAscii m_asciiPacketHeader;
		list<sPacketLogInfo*> m_packets; // vector -> list (for streaming)

		// streaming
		StrPath m_fileName;
		std::streampos m_readPos;
		int m_maxFileSize; // default: 500, (0:infinite packets)
		list<sPacketLogInfo*>::iterator m_searchItor; // ReadPacket() fast search itor
		common::cMemoryPool3<cPacket> m_packetMemPool;
	};

}
