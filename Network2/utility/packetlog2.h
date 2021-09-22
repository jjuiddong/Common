//
// 2021-09-21, jjuiddong
// Packet Log Reader Second version
//	- upgrade cPacketLog
//	- binary streaming
//	- read packet logfile writed network2::LogPacket2()
//
#pragma once


namespace network2
{

	class cPacketLog2
	{
	public:
		struct sPacketInfo
		{
			uint64 dateTime; // yyyymmddhhmmssmmm
			netid sndId;
			netid rcvId;
			uint size;
		};

		cPacketLog2();
		virtual ~cPacketLog2();

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
