
#include "stdafx.h"
#include "packetlog.h"


using namespace network2;

cPacketLog::cPacketLog()
	: m_id(0)
	, m_isCompareDisplay(false)
	, m_asciiPacketHeader(1000)
	, m_readPos(0)
	, m_maxFileSize(500)
	, m_searchItor(m_packets.begin())
{
}

cPacketLog::~cPacketLog()
{
	Clear();
}


// read *.plog file
bool cPacketLog::Read(const char *fileName)
{
	Clear();

	m_fileName = fileName;

	using namespace std;
	ifstream ifs(fileName, ios::binary);
	if (!ifs.is_open())
		return false;

	if (m_readPos == std::streampos(0))
		m_readPos = GetReadPos(fileName);
	
	if (!ReadStream(ifs))
		return false;

	m_totalCount = (int)m_packets.size();
	return true;
}


// read *.plog file all packet
bool cPacketLog::ReadAll(const char *fileName)
{
	Clear();

	m_fileName = fileName;

	using namespace std;
	ifstream ifs(fileName, ios::binary);
	if (!ifs.is_open())
		return false;

	m_maxFileSize = 0; // read infinite packet
	m_readPos = std::streampos(0);

	if (!ReadStream(ifs))
		return false;

	m_totalCount = (int)m_packets.size();
	return true;
}


// return packet near dateTime
// dateTime: yyyymmddhhmmssmmm
// return : sPacketLog reference pointer
cPacketLog::sPacketLogInfo* cPacketLog::ReadPacket(const uint64 dateTime)
{
	uint64 prevGap = 0;
	auto itor = m_searchItor;
	if (m_packets.end() == m_searchItor)
		itor = m_packets.begin();

	int state = 0; // 0:search, 1:goto next, 2:goto prev
	sPacketLogInfo *ret = nullptr;
	sPacketLogInfo *prevLog = nullptr;
	auto prevItor = itor;
	while (m_packets.end() != itor)
	{
		sPacketLogInfo *plog = *itor;

		const __int64 gap = (__int64)plog->dateTime - (__int64)dateTime;
		const uint64 gapAbs = llabs(gap);
		if (gap == 0)
		{
			ret = plog;
			break;
		}

		// move cursor decrease time
		if (0 == state) // check next, previous?
		{
			if (gap < 0)
			{
				state = 1; // goto next
			}
			else // gap > 0
			{
				state = 2; // goto prev
			}
		}
		else
		{
			if (prevGap < gapAbs)
			{
				ret = prevLog;
				break;
			}

			if (1 == state)
			{
				prevItor = itor;
				++itor; // goto next
			}
			else
			{
				if (itor == m_packets.begin())
					break;

				prevItor = itor;
				--itor; // goto prev
			}
		}

		prevGap = gapAbs;
		prevLog = plog;
	}

	if (!ret && !m_packets.empty())
	{
		if (1 == state) // goto next
			ret = m_packets.back();
		else if (2 == state) // goto prev
			ret = m_packets.front();
	}

	if (m_packets.end() == itor)
		m_searchItor = m_packets.end();
	else
		m_searchItor = prevItor;

	return ret;
}


// return packet time range in begin - end
// beginDateTime, endDateTime: yyyymmddhhmmssmmm
// out: sPacketLog reference pointer
bool cPacketLog::ReadPacket(const uint64 beginDateTime, const uint64 endDateTime
	, OUT vector<sPacketLogInfo*> &out)
{
	uint64 prevGap = 0;
	auto itor = m_searchItor;
	if (m_packets.end() == m_searchItor)
		itor = m_packets.begin();

	int state = 0; // 0:check begin, 1:check end
	sPacketLogInfo *ret = nullptr;
	sPacketLogInfo *prevLog = nullptr;
	auto prevItor = itor;
	__int64 dateTime = beginDateTime;

	while (m_packets.end() != itor)
	{
		sPacketLogInfo *plog = *itor;
		const __int64 dt = (__int64)plog->dateTime - (__int64)dateTime;

		if (0 == state) // compare begin time
		{
			if (0 == dt)
			{
				out.push_back(plog);
				dateTime = endDateTime;
				state = 1;
				++itor;
			}
			else if (0 < dt)
			{
				dateTime = endDateTime;
				state = 1;
			}
			else
			{
				++itor;
			}
		}
		else // compare end time
		{
			if (dt <= 0)
			{
				out.push_back(plog);
				++itor;
			}
			else
			{
				break; // finish
			}
		}
	}

	if (m_packets.end() == itor)
		m_searchItor = m_packets.end();
	else
		m_searchItor = itor;

	return true;
}


// File Streaming
// Streaming Process : Read() -> while(1){ Streaming(); }
uint cPacketLog::Streaming()
{
	if (m_fileName.empty())
		return false;

	using namespace std;
	ifstream ifs(m_fileName.c_str(), ios::binary);
	if (!ifs.is_open())
		return false;

	return ReadStream(ifs);
}


// return logfile time range
// return time range, begin time, end time
cDateTimeRange cPacketLog::GetTimeRange(const char *fileName)
{
	using namespace std;
	ifstream ifs(fileName, ios::binary);
	if (!ifs.is_open())
		return { 0, 0 };

	uint64 beginTime = 0;
	uint64 endTime = 0;
	BYTE tmpBuff[DEFAULT_PACKETSIZE];

	while (!ifs.eof())
	{
		sPacketLogInfo tmp;
		tmp.size = 0;
		ifs.read((char*)&tmp.dateTime, sizeof(tmp.dateTime));
		ifs.read((char*)&tmp.rcvId, sizeof(tmp.rcvId));
		ifs.read((char*)&tmp.sndId, sizeof(tmp.sndId));
		ifs.read((char*)&tmp.size, sizeof(tmp.size));
		if (tmp.size == 0)
		{
			break; // eof
		}
		ifs.read((char*)tmpBuff, tmp.size);

		if (0 == beginTime)
			beginTime = tmp.dateTime;
		else
			endTime = tmp.dateTime;
	}

	return cDateTimeRange(beginTime, endTime);
}


// return logfile time range (optimize version)
// return time range, begin time, end time
cDateTimeRange cPacketLog::GetTimeRange2(const char *fileName)
{
	using namespace std;
	ifstream ifs(fileName, ios::binary);
	if (!ifs.is_open())
		return { 0, 0 };

	uint64 beginTime = 0; // yyyymmddhhmmssmmm
	uint64 endTime = 0; // yyyymmddhhmmssmmm

	std::streampos pos(0);
	while (!ifs.eof())
	{
		sPacketLogInfo tmp;
		tmp.size = 0;
		ifs.read((char*)&tmp.dateTime, sizeof(tmp.dateTime));

		pos += sizeof(tmp.dateTime) + sizeof(tmp.rcvId) + sizeof(tmp.sndId);
		ifs.seekg(pos, std::ios::beg);

		ifs.read((char*)&tmp.size, sizeof(tmp.size));
		if (tmp.size == 0)
		{
			break; // eof
		}
		pos += tmp.size + sizeof(tmp.size);
		ifs.seekg(pos, std::ios::beg);

		if (0 == beginTime)
			beginTime = tmp.dateTime;
		else
			endTime = tmp.dateTime;
	}

	return cDateTimeRange(beginTime, endTime);
}


// write packetlog data to file
bool cPacketLog::Write(const char *fileName, const sPacketLogData &logData)
{
	std::ofstream ofs(fileName, std::ios::binary | std::ios::app);
	if (!ofs.is_open())
	{
		assert(0);
		return false;
	}

	const uint64 timeNum = common::GetCurrentDateTime3();
	ofs.write((const char*)&timeNum, sizeof(timeNum));
	ofs.write((const char*)&logData.rcvId, sizeof(logData.rcvId));
	ofs.write((const char*)&logData.sndId, sizeof(logData.sndId));
	ofs.write((const char*)&logData.size, sizeof(logData.size));
	ofs.write((const char*)&logData.data, logData.size);

	return true;
}


// read input stream
uint cPacketLog::ReadStream(std::istream &ifs)
{
	using namespace std;

	if (m_readPos != std::streampos(0))
		ifs.seekg(m_readPos, std::ios::beg);

	BYTE tmpBuff[DEFAULT_PACKETSIZE];
	list<sPacketLogInfo*> logs; // temporal buffer
	while (!ifs.eof())
	{
		sPacketLogInfo tmp;
		tmp.size = 0;

		ifs.read((char*)&tmp.dateTime, sizeof(tmp.dateTime));
		ifs.read((char*)&tmp.rcvId, sizeof(tmp.rcvId));
		ifs.read((char*)&tmp.sndId, sizeof(tmp.sndId));
		ifs.read((char*)&tmp.size, sizeof(tmp.size));
		if (tmp.size == 0)
		{
			break; // eof
		}

		ifs.read((char*)tmpBuff, tmp.size);

		sPacketLogInfo *plog = new sPacketLogInfo;

		// Check Ascii or Binary or Json Format Packet (tricky code)
		{
			//const bool isAsciiFormat = (isalpha(tmpBuff[0]) > 0) 
			//	&& (isalpha(tmpBuff[1]) > 0)
			//	&& (isalpha(tmpBuff[2]) > 0);
			//cPacket *packet = m_packetMemPool.Alloc();
			//iPacketHeader *packetHeader = (isAsciiFormat ?
			//	(iPacketHeader*)&m_asciiPacketHeader : (iPacketHeader*)&m_binPacketHeader);
			//packet->m_packetHeader = packetHeader;
			//packet->m_writeIdx = tmp.size;
			//packet->m_readIdx = tmp.size;
			//plog->packet = packet;
		}

		cPacket *packet = m_packetMemPool.Alloc();
		packet->m_packetHeader = &m_binPacketHeader; // default binary
		packet->m_writeIdx = tmp.size;
		packet->m_readIdx = tmp.size;
		plog->packet = packet;

		plog->dateTime = tmp.dateTime;
		plog->rcvId = tmp.rcvId;
		plog->sndId = tmp.sndId;
		plog->size = tmp.size;
		plog->packet->m_sndId = plog->sndId;
		memcpy(plog->packet->m_data, tmpBuff, plog->size);

		const ePacketFormat format = network2::GetPacketFormat(*packet);
		switch (format)
		{
		case ePacketFormat::BINARY: packet->m_packetHeader = &m_binPacketHeader; break;
		case ePacketFormat::ASCII: packet->m_packetHeader = &m_asciiPacketHeader; break;
		case ePacketFormat::JSON: packet->m_packetHeader = &m_jsonPacketHeader; break;
		default: assert(0); break;
		}

		// get packet name
		plog->name = network2::GetPacketName(*plog->packet);
		plog->nameHashcode = plog->name.GetHashCode();

		// get packet information string
		network2::GetPacketString(*plog->packet, plog->packetInfo);

		logs.push_back(plog);

		m_readPos = ifs.tellg(); // last file pointer
	}

	// when streaming packetlog file, logs read different data
	// and then m_packets update and remove
	for (auto &p : logs)
	{
		while ((m_maxFileSize != 0)
			&& ((int)m_packets.size() >= m_maxFileSize))
		{
			auto *packet = m_packets.front();
			m_packetMemPool.Free(packet->packet);
			delete packet;
			m_packets.pop_front();
		}

		m_packets.push_back(p);
	}

	return logs.size();
}


// return file pointer that m_maxFileSize size store
std::streampos cPacketLog::GetReadPos(const char *fileName)
{
	using namespace std;
	ifstream ifs(fileName, ios::binary);
	if (!ifs.is_open())
		return false;

	BYTE tmpBuff[DEFAULT_PACKETSIZE];
	list<std::streampos> poss;
	while (!ifs.eof())
	{
		poss.push_back(ifs.tellg());

		sPacketLogInfo tmp;
		tmp.size = 0;
		ifs.read((char*)&tmp.dateTime, sizeof(tmp.dateTime));
		ifs.read((char*)&tmp.rcvId, sizeof(tmp.rcvId));
		ifs.read((char*)&tmp.sndId, sizeof(tmp.sndId));
		ifs.read((char*)&tmp.size, sizeof(tmp.size));
		if (tmp.size == 0)
		{
			break; // eof
		}
		ifs.read((char*)tmpBuff, tmp.size);
	}

	int count = 0;
	std::streampos pos = 0;
	auto it = poss.rbegin();
	while ((poss.rend() != it) && (count < m_maxFileSize))
	{
		pos = *it++;
		++count;
	}
	return pos;
}


// initialize search cursor, search cursor move to begin
void cPacketLog::ClearSearchCursor()
{
	m_searchItor = m_packets.begin();
}


void cPacketLog::Clear()
{
	for (auto &p : m_packets)
	{
		m_packetMemPool.Free(p->packet);
		delete p;
	}
	m_packets.clear();
	m_searchItor = m_packets.begin();
	m_readPos = 0;
}
