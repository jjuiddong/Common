
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


// dateTime과 가장 가까운 시간의 패킷정보를 리턴한다.
// dateTime: yyyymmddhhmmssmmm
// return : sPacketLog reference pointer
cPacketLog::sPacketLogInfo* cPacketLog::ReadPacket(const uint64 dateTime)
{
	uint64 timeSearch = dateTime;
	uint64 prevGap = 0;
	auto itor = m_searchItor;
	if (m_packets.end() == m_searchItor)
		itor = m_packets.begin();

	int state = 0; // 0:search, 1:goto next, 2:goto prev
	sPacketLogInfo *ret = NULL;
	sPacketLogInfo *prevLog = NULL;
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

		// 시간차가 줄어드는 방향으로 이동한다.
		if (0 == state)
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


// 로그에 저장된 패킷 시간 범위를 리턴한다.
// firsst : begin time
// second : end time
cDateTimeRange cPacketLog::GetTimeRange(const char *fileName)
{
	using namespace std;
	ifstream ifs(fileName, ios::binary);
	if (!ifs.is_open())
		return { 0, 0 };

	uint64 beginTime = 0;
	uint64 endTime = 0;

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
		BYTE tmpBuff[DEFAULT_PACKETSIZE];
		ifs.read((char*)tmpBuff, tmp.size);

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

		BYTE tmpBuff[DEFAULT_PACKETSIZE];
		ifs.read((char*)tmpBuff, tmp.size);

		sPacketLogInfo *plog = new sPacketLogInfo;

		// Check Ascii or Binary Format Packet (꽁수 코드)
		{
			const bool isAsciiFormat = isalpha(tmpBuff[0]) > 0;
			cPacket *packet = m_packetMemPool.Alloc();
			iPacketHeader *packetHeader = (isAsciiFormat ?
				(iPacketHeader*)&m_asciiPacketHeader : (iPacketHeader*)&m_binPacketHeader);
			packet->m_packetHeader = packetHeader;
			packet->m_writeIdx = packetHeader->GetHeaderSize();
			packet->m_readIdx = packetHeader->GetHeaderSize();

			plog->packet = packet;
		}

		plog->dateTime = tmp.dateTime;
		plog->rcvId = tmp.rcvId;
		plog->sndId = tmp.sndId;
		plog->size = tmp.size;
		plog->packet->m_sndId = plog->sndId;
		memcpy(plog->packet->m_data, tmpBuff, plog->size);

		// get packet name
		plog->name = network2::GetPacketName(*plog->packet);
		plog->nameHashcode = plog->name.GetHashCode();

		// get packet information string
		network2::GetPacketString(*plog->packet, plog->packetInfo);

		logs.push_back(plog);

		m_readPos = ifs.tellg(); // last file pointer
	}

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


// m_maxFileSize 크기만큼 패킷정보를 파일에서 읽을 때, 
// 최대한 읽을 수 있는 위치의 파일 포인터를 리턴한다.
std::streampos cPacketLog::GetReadPos(const char *fileName)
{
	using namespace std;
	ifstream ifs(fileName, ios::binary);
	if (!ifs.is_open())
		return false;

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
		BYTE tmpBuff[DEFAULT_PACKETSIZE];
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
