#include "stdafx.h"
#include "packetheaderascii.h"
#include "marshalling_ascii.h"

using namespace network2;


cPacketHeaderAscii::cPacketHeaderAscii(const int protocolId //= 100
)
	: m_protocolId(protocolId)
{
}

// return packet header size
uint cPacketHeaderAscii::GetHeaderSize()
{
	return sizeof(int) * 2;
}


int cPacketHeaderAscii::GetProtocolId(const BYTE *src)
{
	return m_protocolId;
}

uint cPacketHeaderAscii::GetPacketId(const BYTE *src)
{
	return *(uint*)(src);
}


uint cPacketHeaderAscii::GetPacketLength(const BYTE *src)
{
	const char *ptr = (const char*)(src + sizeof(int));
	uint size = 0;
	size += (ptr[0] - '0') * 1000;
	size += (ptr[1] - '0') * 100;
	size += (ptr[2] - '0') * 10;
	size += (ptr[3] - '0') * 1;
	return size;
}


void cPacketHeaderAscii::SetProtocolId(BYTE *dst, const int protocolId)
{
	// nothing
}


void cPacketHeaderAscii::SetPacketId(BYTE *dst, const uint packetId)
{
	*(uint*)dst = packetId;
}


void cPacketHeaderAscii::SetPacketLength(BYTE *dst, const uint packetLength)
{
	const uint size = packetLength;
	dst[4] = min(9, size / 1000) + '0';
	dst[5] = min(9, (size % 1000) / 100) + '0';
	dst[6] = min(9, (size % 100) / 10) + '0';
	dst[7] = min(9, size % 10) + '0';
}


int cPacketHeaderAscii::SetPacketTerminator(BYTE *dst, const int remainSize)
{
	int cnt = 0;
	for (int i = 0; i < 4 && (remainSize - i > 0); ++i)
	{
		dst[i] = '@';
		++cnt;
	}
	return cnt;
}


int cPacketHeaderAscii::SetDelimeter(BYTE *dst)
{
	*dst = marshalling_ascii::DELIMETER;
	return 1;
}


bool cPacketHeaderAscii::IsValidPacket(const BYTE *src)
{
	return (GetProtocolId(src) >= 0)
		&& (GetPacketId(src) >= 0)
		&& (GetPacketLength(src) > 0);
	return true;
}
