
#include "stdafx.h"
#include "packetpump.h"

using namespace network2;


cPacketPump::cPacketPump()
{
}

cPacketPump::~cPacketPump()
{
	Clear();
}


// read packet log from 'directoryPath' directory
// directoryPath: saved packet log file directory name
bool cPacketPump::ReadPacketLog(const StrPath &directoryPath)
{
	Clear();

	m_fileName = directoryPath;

	// read sessions.ses, session information file
	// ex) 10001.plog : packet logfile netid 10001 
	if (!IsFileExist(directoryPath + "/sessions.ses"))
	{
		// Not Found session.ses file
		return false;
	}
	if (!m_sessionsLog.Read((directoryPath + "/sessions.ses").c_str()))
	{
		// Error Read sessions.ses file
		return false;
	}

	// read *.plog
	{
		list<string> files;
		list<string> exts;
		exts.push_back(".plog");
		common::CollectFiles(exts, directoryPath.c_str(), files);

		m_timeRange.SetTimeRange(0, 0);
		for (auto &file : files)
		{
			const netid netId = atoi(common::GetFileName(file).c_str());
			network2::cPacketLog *packetLog = new network2::cPacketLog();
			packetLog->m_id = netId;
			packetLog->Read(file.c_str());

			const auto result = packetLog->GetTimeRange(file.c_str());
			m_timeRange = m_timeRange.Max(result);

			m_plogs.insert({ netId, packetLog });
		}
	}

	// update PacketLog Name
	for (auto &plog : m_plogs.m_seq)
	{
		const StrId &name = m_sessionsLog.FindSession(plog->m_id);
		plog->m_name = name;
	}

	m_timeLine.SetTime(m_timeRange.m_first);

	return true;
}


// out1: packet log data array
// out2: packet log data array, changed packet
// out3: out1 packet data ptr
// out4: out2 packet data ptr
// return: has log data?
bool cPacketPump::Update(const float deltaSeconds
	, OUT vector<network2::cPacketLog::sPacketInfo*> &out1
	, OUT vector<network2::cPacketLog::sPacketInfo*> &out2
	, OUT vector<BYTE*> &out3
	, OUT vector<BYTE*> &out4
	, OUT vector<network2::cPacketLog*> &out1PacketLogs
	, OUT vector<network2::cPacketLog*> &out2PacketLogs
)
{
	if (m_plogs.empty())
		return false;

	out1.clear();
	out1PacketLogs.clear();

	// calc time range (begin ~ end)
	const uint64 dt = (uint64)(deltaSeconds * 1000.f);
	const uint64 bt = m_timeLine.m_t;
	uint64 beginT = m_timeLine.GetTimeInt64(); // yyyymmddhhmmssmmm
	m_timeLine += dt;
	if (m_timeRange.m_second <= m_timeLine)
		m_timeLine = m_timeRange.m_second;
	const uint64 endT = m_timeLine.GetTimeInt64(); // yyyymmddhhmmssmmm

	if (llabs(m_timeLine.m_t - bt) > 1000) // 1000 millisecond over?
	{
		cDateTime2 t = m_timeLine;
		t -= (uint64)1000; // maximum gap = 1000 milliseconds
		beginT = t.GetTimeInt64(); // yyyymmddhhmmssmmm
	}

	vector<vector<network2::cPacketLog::sPacketInfo*>> ars1(m_plogs.size());
	vector<vector<BYTE*>> ars2(m_plogs.size());
	for (uint i=0; i < m_plogs.size(); ++i)
	{
		network2::cPacketLog *plog = m_plogs.m_seq[i];
		vector<network2::cPacketLog::sPacketInfo*> infos;
		vector<BYTE*> ptrs;
		plog->ReadPacket(beginT, endT, infos, ptrs);
		for (auto &p : infos)
		{
			out1.push_back(p);
			out1PacketLogs.push_back(plog);
			ars1[i].push_back(p);
		}
		for (auto& p : ptrs)
		{
			out3.push_back(p);
			ars2[i].push_back(p);
		}
	}

	// change packet store in out2
	vector<vector<network2::cPacketLog::sPacketInfo*>> ars3(m_plogs.size());
	vector<vector<BYTE*>> ars4(m_plogs.size());
	for (uint i = 0; i < ars1.size(); ++i)
	{
		const vector<network2::cPacketLog::sPacketInfo*>& ar1 = ars1[i];
		const vector<BYTE*>& ar2 = ars2[i];
		for (uint k = 0; k < ar1.size(); ++k)
		{
			auto &log = ar1[k];
			auto it = m_timeLines.find(log->dateTime);
			if (m_timeLines.end() == it)
			{
				ars3[i].push_back(log);
				ars4[i].push_back(ar2[k]);
			}
		}
	}

	m_timeLines.clear();
	for (auto &p : out1)
		m_timeLines.insert(p->dateTime);

	size_t totalSize = 0;
	for (uint i = 0; i < ars3.size(); ++i)
		totalSize += ars3[i].size();
	out2.reserve(totalSize);
	out4.reserve(totalSize);
	out2PacketLogs.reserve(totalSize);

	// sorting by dateTime
	vector<uint> indices(ars3.size(), 0);
	while (1)
	{
		int minIdx1 = -1;
		int minIdx2 = -1;
		uint64 minDateTime = ULLONG_MAX;

		for (uint i = 0; i < ars3.size(); ++i)
		{
			if (ars3[i].size() <= indices[i]) continue; // all saved
			const uint64 dateTime = ars3[i][indices[i]]->dateTime;
			if (minDateTime > dateTime)
			{
				minDateTime = dateTime;
				minIdx1 = (int)i;
				minIdx2 = (int)indices[i];
			}
		}
		if (minIdx1 < 0)
			break; // finish

		out2.push_back(ars3[minIdx1][minIdx2]);
		out4.push_back(ars4[minIdx1][minIdx2]);
		out2PacketLogs.push_back(m_plogs.m_seq[minIdx1]);
		++indices[minIdx1];
	}

	return true;
}


void cPacketPump::SetTimeLine(const cDateTime2 &dateTime)
{
	if (m_timeLine == dateTime)
		return;
	m_timeLine = dateTime;

	// search cursor initialize
	for (auto &plog : m_plogs.m_seq)
		plog->SetTimeLine(dateTime.GetTimeInt64());
}


// goto start point
void cPacketPump::SetBeginTimeLine()
{
	SetTimeLine(m_timeRange.m_first);
}


// return total begin to end time range
common::cDateTimeRange cPacketPump::GetTimeRange(const StrPath &directoryPath)
{
	// read *.plog files
	list<string> files;
	list<string> exts;
	exts.push_back(".plog");
	common::CollectFiles(exts, directoryPath.c_str(), files);

	cDateTimeRange trange;
	network2::cPacketLog plog;
	for (auto &file : files)
	{
		const auto result = plog.GetTimeRange(file.c_str());
		trange = trange.Max(result);
	}
	return trange;
}


// return progress ratio, 0 ~ 1.0
float cPacketPump::GetProgress()
{
	const uint64 pos = m_timeLine.m_t - m_timeRange.m_first.m_t;
	const uint64 range = m_timeRange.m_second.m_t - m_timeRange.m_first.m_t;
	const double ratio = (double)pos / (double)range;
	return (float)ratio;
}


// is packet pump finish?
bool cPacketPump::IsEnd()
{
	return m_timeRange.m_second.m_t <= m_timeLine.m_t;
}


void cPacketPump::Clear()
{
	m_sessionsLog.Clear();
	for (auto &p : m_plogs.m_seq)
		delete p;
	m_plogs.clear();
	m_timeLines.clear();
	m_fileName.clear();
}
