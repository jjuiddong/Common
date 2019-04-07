
#include "stdafx.h"
#include "socketbuffer.h"


using namespace network2;

cSocketBuffer::cSocketBuffer(const netid netId)
	: m_netId(netId)
	, m_readLen(0)
	, m_totalLen(0)
	, m_isHeaderCopy(false)
{
}

cSocketBuffer::~cSocketBuffer()
{
	Clear();
}


uint cSocketBuffer::Push(iPacketHeader *packetHeader, const BYTE *data, const uint size)
{
	uint dataSize = size;
	const uint headerSize = packetHeader->GetHeaderSize();

	if (m_isHeaderCopy)
	{
		const uint cpSize = min(size, sizeof(m_tempHeader) - m_readLen);
		memcpy(m_tempHeader + m_readLen, data, cpSize);
		m_isHeaderCopy = false;
		data = m_tempHeader;
		dataSize = size + m_readLen;
		m_readLen = 0;
	}

	uint offset = 0;
	while (offset < dataSize)
	{
		const BYTE *ptr = data + offset;
		const uint len = max(0, dataSize - offset);

		if (m_readLen > 0)
		{
			// remain receive packet
			const uint cpSize = min(m_totalLen - m_readLen, len);
			const uint pushSize = m_q.push(ptr, cpSize);

			if (pushSize != cpSize)
			{
				// error occur
				// queue memory full
				break;
			}

			if (m_totalLen == (pushSize + m_readLen))
			{
				m_readLen = 0;
			}
			else
			{
				m_readLen += pushSize;
			}

			offset += cpSize;
		}
		else if (len < headerSize)
		{
			// need full packet hearder
			m_isHeaderCopy = true;
			memcpy(m_tempHeader, ptr, len);
			m_readLen = len;
			offset += len;
		}
		else
		{
			const uint totalLen = packetHeader->GetPacketLength(ptr);
			if (totalLen > DEFAULT_PACKETSIZE)
				break; // error occur, packet size error, maybe data corrupt

			const uint cpSize = min(totalLen, len);
			const uint pushSize = m_q.push(ptr, cpSize);
			if (pushSize != cpSize)
			{
				// error occur
				// queue memory full
				break;
			}

			if (totalLen == pushSize)
			{
				m_readLen = 0;
			}
			else
			{
				m_totalLen = totalLen;
				m_readLen = pushSize;
			}

			offset += cpSize;
		}
	}

	return offset;
}


bool cSocketBuffer::Pop(cPacket &out)
{
	RETV(m_q.empty(), false);

	const uint size = m_q.size();
	BYTE tempHeader[64];
	const uint headerSize = out.m_packetHeader->GetHeaderSize();
	if (!m_q.front(tempHeader, headerSize))
		return false;

	const uint packetSize = out.m_packetHeader->GetPacketLength(tempHeader);
	if ((packetSize <= 0) || (packetSize >= DEFAULT_SOCKETBUFFER_SIZE))
	{
		std::cout << "socketbuffer clear" << std::endl;
		m_q.clear();
		return false;
	}

	if (packetSize <= size)
	{
		out.m_writeIdx = packetSize;
		m_q.frontPop(out.m_data, min(sizeof(out.m_data), packetSize));
		return true;
	}

	return false;
}


void cSocketBuffer::Clear()
{
	m_totalLen = 0;
	m_readLen = 0;
	m_q.clear();
}
