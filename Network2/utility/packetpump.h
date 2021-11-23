//
// 2021-09-21, jjuiddong
// PacketPump from PacketLog File
//
#pragma once


namespace network2
{

	class cPacketPump
	{
	public:
		cPacketPump();
		virtual ~cPacketPump();

		bool ReadPacketLog(const StrPath &directoryPath);
		bool Update(const float deltaSeconds
			, OUT vector<network2::cPacketLog::sPacketInfo*> &out1
			, OUT vector<network2::cPacketLog::sPacketInfo*> &out2
			, OUT vector<BYTE*> &out3
			, OUT vector<BYTE*> &out4
			, OUT vector<network2::cPacketLog*> &out1PacketLogs
			, OUT vector<network2::cPacketLog*> &out2PacketLogs
		);
		void SetTimeLine(const cDateTime2 &dateTime);
		void SetBeginTimeLine();
		common::cDateTimeRange GetTimeRange(const StrPath &directoryPath);
		float GetProgress();
		bool IsEnd();
		void Clear();


	public:
		StrPath m_fileName; // packet log information filename
		network2::cSessionLog m_sessionsLog;
		VectorMap<netid, network2::cPacketLog*> m_plogs; // packet log array
		set<uint64> m_timeLines; // dateTime(yyyymmddhhmmssmmm) array
		common::cDateTimeRange m_timeRange; // total packet log time range
		cDateTime2 m_timeLine; // current update dateTime
	};

}
