
#include "stdafx.h"
#include "packetlog.h"

using namespace network2;

namespace
{
	// packet log header
	// to fast detect packet information, we need mark
	static const uint64 g_logHeader = 0xf1f2f3f4f5f6f7f8;
}


cPacketLog::cPacketLog()
	: m_cursor(BUFFER_SIZE)
	, m_isEof(false)
	, m_fileSize(0)
{
	m_buffer = new BYTE[BUFFER_SIZE];
}

cPacketLog::~cPacketLog()
{
	Clear();
	SAFE_DELETEA(m_buffer);
}


// read packet log file, *.plog
bool cPacketLog::Read(const StrPath &fileName)
{
	Clear();

	if (!fileName.IsFileExist())
		return false;

	m_fileName = fileName;
	m_fileSize = fileName.FileSize();
	m_readPos = std::streampos(0);
	if (1 != Load())
		return false; // error return
	
	m_timeRange = GetTimeRange(fileName);
	return true;
}


// return packet time range in begin - end
// beginDateTime, endDateTime: yyyymmddhhmmssmmm
// out1: packet info reference pointer array
// out2: packet data pointer array
bool cPacketLog::ReadPacket(const uint64 beginDateTime, const uint64 endDateTime
	, OUT vector<sPacketInfo*> &out1, OUT vector<BYTE*> &out2)
{
	int state = 0; // 0:check begin, 1:check end
	__int64 dateTime = beginDateTime;

	while (1)
	{
		if (!m_isEof && (0 == state) && (m_cursor > (int)((float)BUFFER_SIZE * 0.6f)))
			if (1 != Load())
				break;

		if (((int)BUFFER_SIZE - m_cursor) < (int)sizeof(g_logHeader))
			break; // end of file

		// check log header
		if (!IsLogHeader(m_buffer + m_cursor))
		{
			if (m_isEof)
			{
				const int64 start = 
					std::max((int64)m_readPos - (int64)BUFFER_SIZE, (int64)0);
				const int64 pos = start + (int64)m_cursor;
				if (m_fileSize <= pos)
				{
					m_cursor = BUFFER_SIZE;
					break; // end of file
				}
			}
			++m_cursor;
			continue;
		}
		m_cursor += sizeof(g_logHeader);

		if (((int)BUFFER_SIZE - m_cursor) < (int)sizeof(sPacketInfo))
		{
			if (!m_isEof)
				m_cursor -= sizeof(g_logHeader); // goto packet start
			break; // end of file
		}

		sPacketInfo *info = (sPacketInfo*)(m_buffer + m_cursor);
		if (!IsValidDateTime(info->dateTime))
			break; // data corruption
		m_cursor += sizeof(sPacketInfo);

		const int64 dt = (int64)info->dateTime - (int64)dateTime;

		if (0 == state) // compare begin time
		{
			if (0 <= dt)
			{
				if (info->dateTime < endDateTime)
				{
					out1.push_back(info);
					out2.push_back(m_buffer + m_cursor); // packet data pointer
				}
				else
				{
					// goto packet start
					m_cursor -= (sizeof(sPacketInfo) + sizeof(g_logHeader));
					break; // finish
				}
				dateTime = endDateTime;
				state = 1;
				m_cursor += info->size; // jump packet size (goto next packet)
			}
			else
			{
				m_cursor += info->size; // jump packet size (goto next packet)
			}
		}
		else // compare end time
		{
			if (dt <= 0)
			{
				if (BUFFER_SIZE < (m_cursor + info->size))
				{
					// goto packet start
					if (!m_isEof)
						m_cursor -= (sizeof(sPacketInfo) + sizeof(g_logHeader));
					break; // end of buffer
				}

				out1.push_back(info);
				out2.push_back(m_buffer + m_cursor); // packet data pointer
				m_cursor += info->size; // jump packet size (goto next packet)
			}
			else
			{
				// goto packet start
				m_cursor -= (sizeof(sPacketInfo) + sizeof(g_logHeader));
				break; // finish
			}
		}
	}

	return true;
}


// update streaming buffer
// dateTime: yyyymmddhhmmssmmm
bool cPacketLog::SetTimeLine(const uint64 dateTime)
{
	if (dateTime <= m_timeRange.m_first.GetTimeInt64())
	{
		// goto front
		m_readPos = 0;
		m_cursor = BUFFER_SIZE;
		m_isEof = false;
		return true;
	}

	bool isFront = false;
	int state = -1; // -1:none, 0:search backward, 1:search forward
	while (1)
	{
		if (!m_isEof && (0 != state) && (m_cursor > (int)((float)BUFFER_SIZE * 0.6f)))
			if (1 != Load())
				break;

		if (((int)BUFFER_SIZE - m_cursor) < (int)sizeof(g_logHeader))
			break; // end of file

		// check log header
		if (!IsLogHeader(m_buffer + m_cursor))
		{
			++m_cursor;
			continue;
		}
		m_cursor += sizeof(g_logHeader);

		if (((int)BUFFER_SIZE - m_cursor) < (int)sizeof(sPacketInfo))
		{
			if (!m_isEof)
				continue; // read next data
			break; // end of file
		}

		sPacketInfo *info = (sPacketInfo*)(m_buffer + m_cursor);
		if (!IsValidDateTime(info->dateTime))
			break; // data corruption

		const int64 dt = (int64)info->dateTime - (int64)dateTime;
		if (-1 == state)
		{
			// determine search backward or forward
			if ((dt <= 0) && (dt > -1000))
			{
				// update timeLine streaming buffer
				// complete
				break;
			}
			else if (dt < 0)
			{
				state = 1; // search forward
			}
			else
			{
				// search backward
				state = 0;
				m_isEof = false;
				m_cursor = BUFFER_SIZE;
				if (0 == m_readPos)
					break; // complete
				uint readPos = 0;
				if ((int64)m_readPos - (int64)(BUFFER_SIZE * 2) < 0)
					readPos = 0;
				else
					readPos = (uint)m_readPos - (BUFFER_SIZE * 2);
				isFront = readPos == 0;
				m_readPos = readPos;
				if (1 != Load())
					break;
			}
		}
		else if (0 == state)
		{
			// search backward
			if (dt > 0)
			{
				// search backward
				if (isFront)
					break; // no more backward to
				m_isEof = false;
				m_cursor = BUFFER_SIZE;
				if (0 == m_readPos)
					break; // complete
				uint readPos = 0;
				if ((int64)m_readPos - (int64)(BUFFER_SIZE * 2) < 0)
					readPos = 0;
				else
					readPos = (uint)m_readPos - (BUFFER_SIZE * 2);
				isFront = readPos == 0;
				m_readPos = readPos;
				if (1 != Load())
					break;
			}
			else
			{
				state = 1; // search forward
			}
		}
		else
		{
			// search forward
			if (dt >= 0)
			{
				// update timeLine streaming buffer
				// complete
				m_cursor -= sizeof(g_logHeader); // goto packet start
				break;
			}

			m_cursor += sizeof(sPacketInfo);
			m_cursor += info->size; // jump packet size (goto next packet)
		}
	}//~while

	return true;
}


// return packet log start, end time
cDateTimeRange cPacketLog::GetTimeRange(const StrPath &fileName)
{
	using namespace std;

	cDateTimeRange res;
	ifstream ifs(fileName.c_str(), ios::binary);
	if (!ifs.is_open())
		return res;

	// find start time
	{
		BYTE buffer[128];
		const uint readSize = sizeof(sPacketInfo) + sizeof(g_logHeader);

		ifs.read((char*)buffer, readSize);
		if (!IsLogHeader(buffer))
			return res;
		if (ifs.tellg() < readSize)
			return res;
		sPacketInfo *info = (sPacketInfo*)(buffer + sizeof(g_logHeader));
		if (!IsValidDateTime(info->dateTime))
			return res;
		res.m_first = info->dateTime;
	}//~find start time

	// find end time, read end point
	{
		bool isFind = false; // detect end time data?
		BYTE buffer[256];
		const int hsize = sizeof(buffer) / 2;
		int pos = hsize; // half size
		while (!isFind && (pos < 1024))
		{
			// shift buffer data
			if (hsize != pos) // no first?
				memcpy(buffer + hsize, buffer, hsize);

			// scan file backward
			ifs.seekg(-pos, std::ios::end);
			ifs.read((char*)buffer, hsize);

			// find log header
			uint p = 0;
			while (!isFind && (p < (hsize + sizeof(g_logHeader))))
			{
				if (IsLogHeader(buffer + p))
				{
					// detect log header
					sPacketInfo *info = (sPacketInfo*)(buffer + p + sizeof(g_logHeader));
					if (!IsValidDateTime(info->dateTime))
						return res;
					res.m_second = info->dateTime;
					isFind = true;
					break;
				}
				++p; // next ptr
			}
			pos += hsize;
		}
	}//~find end time

	return res;
}


// load packet logfile to streaming buffer
// return: -1: not found file
//		    0: end of file
//		    1: success load
//			2: full buffer
int cPacketLog::Load()
{
	using namespace std;

	if (m_isEof)
		return 0;

	const uint remainSize = (uint)m_cursor;
	if (0 == remainSize)
		return 2;
	if (remainSize < (BUFFER_SIZE / 2))
		return 1; // no need load

	ifstream ifs(m_fileName.c_str(), ios::binary);
	if (!ifs.is_open())
		return -1; // error read file
	ifs.seekg(m_readPos, ios::beg);

	// move buffer front
	uint writeCursor = 0; // write cursor
	if (m_cursor > 0)
	{
		if (BUFFER_SIZE - m_cursor > 0)
			memcpy(m_buffer, m_buffer + m_cursor, BUFFER_SIZE - m_cursor);
		writeCursor = BUFFER_SIZE - m_cursor;
		m_cursor = 0;
	}

	const std::streampos p0 = ifs.tellg();
	const uint cpySize = (uint)std::min((uint64)remainSize, m_fileSize - m_readPos);
	if (cpySize > 0)
		ifs.read((char*)m_buffer + writeCursor, cpySize);
	const std::streampos p1 = ifs.tellg();

	// calc actual read size
	const uint readSize = (uint)(p1 - p0);
	if ((0 == readSize) || (readSize != cpySize) || (remainSize != cpySize))
	{
		m_isEof = true;
		ZeroMemory(m_buffer + writeCursor + readSize
			, BUFFER_SIZE - (writeCursor + readSize));
	}

	m_readPos = ifs.tellg();
	return 1;
}


// is log header?
bool cPacketLog::IsLogHeader(const BYTE *buffer)
{
	return (*(uint64*)buffer == g_logHeader);
}


// is valid date time?
// dateTime: yyyymmddhhmmssmmm
// dateTime range: 2021-01-01 ~ 2121-01-01  (100 year)
bool cPacketLog::IsValidDateTime(const uint64 dateTime)
{
	return (dateTime > 20210101000000000) && (dateTime < 21210101000000000);
}


void cPacketLog::Clear()
{
	m_readPos = 0;
	m_cursor = BUFFER_SIZE;
	m_isEof = false;
}


// static function, write packetlog data to file
bool cPacketLog::Write(const char *fileName, const sPacketLogData &logData)
{
	std::ofstream ofs(fileName, std::ios::binary | std::ios::app);
	if (!ofs.is_open())
	{
		assert(0);
		return false;
	}

	const uint64 timeNum = common::GetCurrentDateTime3();
	ofs.write((const char*)&g_logHeader, sizeof(g_logHeader)); // add log header

	sPacketInfo info;
	info.dateTime = timeNum;
	info.sndId = logData.sndId;
	info.rcvId = logData.rcvId;
	info.size = logData.size;
	ofs.write((const char*)&info, sizeof(info));
	ofs.write((const char*)&logData.data, logData.size);
	return true;
}
