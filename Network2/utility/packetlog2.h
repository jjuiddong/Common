//
// 2019-03-09, jjuiddong
// Packet Log Reader
//	- *.plog file read
//	- write network2::LogPacket()
//
// 2021-09-21
//	- optimize
//	- binary streaming
//
#pragma once


namespace network2
{

	struct sPacketLogData
	{
		int id; // log id
		int type; // 0:session, 1:packet
		netid sndId; // type=1, sender id
		netid rcvId; // type=1, receiver id
		uint size; // type=1, data size
		BYTE data[DEFAULT_PACKETSIZE];
	};

	class cPacketLog
	{
	public:
		struct sPacketInfo
		{
			uint64 dateTime; // yyyymmddhhmmssmmm
			netid sndId;
			netid rcvId;
			uint size;
		};

		cPacketLog();
		virtual ~cPacketLog();

		bool Read(const StrPath &fileName);
		bool ReadPacket(const uint64 beginDateTime, const uint64 endDateTime
			, OUT vector<sPacketInfo*> &out1, OUT vector<BYTE*> &out2);
		bool SetTimeLine(const uint64 dateTime);
		cDateTimeRange GetTimeRange(const StrPath &fileName);
		void Clear();

		static bool Write(const char *fileName, const sPacketLogData &logData);


	protected:
		int Load();
		bool IsLogHeader(const BYTE *buffer);
		bool IsValidDateTime(const uint64 dateTime);


	public:
		enum {
			BUFFER_SIZE = 1024 * 100
		};

		netid m_id; // session id
		StrId m_name; // session Name
		StrPath m_fileName; // packet log filename

		// streaming information
		std::streampos m_readPos;
		int m_cursor; // buffer cursor
		uint64 m_fileSize; // packet logfile size
		bool m_isEof; // is end of file?
		BYTE *m_buffer; // streaming buffer
		cDateTimeRange m_timeRange; // packet log start, end time range
	};

}
