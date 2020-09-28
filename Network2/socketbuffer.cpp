
#include "stdafx.h"
#include "socketbuffer.h"


using namespace network2;

cSocketBuffer::cSocketBuffer(const netid netId, const int maxBufferSize)
	: m_netId(netId)
	, m_readLen(0)
	, m_totalLen(0)
	, m_isHeaderCopy(false)
	, m_q(maxBufferSize)
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
		const uint len = max((uint)0, dataSize - offset);

		// 패킷이 쪼개져서 들어올 경우 (m_readLen > 0) 상태가 된다.
		// 패킷 하나가 완성되기까지, 몇바이트의 데이타가 읽을게 남았다는 의미.
		if (m_readLen > 0)
		{
			// remain receive packet
			const uint cpSize = min(m_totalLen - m_readLen, len);
			const uint pushSize = m_q.push(ptr, cpSize);

			if (pushSize != cpSize)
			{
				// error occur
				// queue memory full
				dbg::Logc(2, "error!! sockbuffer queue memory full1, size = %d, cpSize = %d, pushSize = %d\n"
					, size, cpSize, pushSize);
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
			// need full packet header
			m_isHeaderCopy = true;
			memcpy(m_tempHeader, ptr, len);
			m_readLen = len;
			offset += len;
		}
		else
		{
			const uint totalLen = packetHeader->GetPacketLength(ptr);
			if (totalLen > (uint)m_q.SIZE)
			{
				dbg::Logc(2, "error!! sockbuffer packet size too big, size = %d, totalLen = %d\n"
					, size, totalLen);
				break; // error occur, packet size error, maybe data corrupt
			}
			else if (totalLen == 0)
			{
				dbg::Logc(2, "error!! sockbuffer packet size zero\n");
				break; // error occur, packet size error, maybe data corrupt
			}

			const uint cpSize = min(totalLen, len);
			const uint pushSize = m_q.push(ptr, cpSize);
			if (pushSize != cpSize)
			{
				// error occur
				// queue memory full
				offset = 0;
				dbg::Logc(2, "error!! sockbuffer queue memory full3, size = %d, cpSize = %d, pushSize = %d\n"
					, size, cpSize, pushSize);
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


// pop buffer and remove 
bool cSocketBuffer::Pop(OUT cPacket &out)
{
	if (PopNoRemove(out))
		return Pop(min(sizeof(out.m_data), (uint)out.m_writeIdx));
	return false;
}


// no remove pop buffer
bool cSocketBuffer::PopNoRemove(OUT cPacket &out)
{
	RETV(m_q.empty(), false);

	const uint size = m_q.size();
	BYTE tempHeader[64];
	const uint headerSize = out.m_packetHeader->GetHeaderSize();
	if (!m_q.frontCopy(tempHeader, headerSize))
		return false;

	const uint packetSize = out.m_packetHeader->GetPacketLength(tempHeader);
	if ((packetSize <= 0) || (packetSize >= DEFAULT_SOCKETBUFFER_SIZE))
	{
		m_q.clear();
		dbg::Logc(2, "Error!! cSocketBuffer::PopNoRemove, PacketHeader error, packetSize = %d\n", packetSize);
		return false;
	}

	if (packetSize <= size)
	{
		out.m_writeIdx = packetSize;
		m_q.frontCopy(out.m_data, min(sizeof(out.m_data), packetSize));
		return true;
	}

	return false;
}


// remove buffer
bool cSocketBuffer::Pop(const uint size)
{
	if (size <= m_q.size())
		m_q.pop(size);
	return true;
}


void cSocketBuffer::Clear()
{
	m_totalLen = 0;
	m_readLen = 0;
	m_q.clear();
}
