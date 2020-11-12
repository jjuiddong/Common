#include "stdafx.h"
#include "packetheader.h"

using namespace network2;


// return packet header size
// protocol ID + packet ID + packet Length
uint cPacketHeader::GetHeaderSize()
{
	return sizeof(int) * 3;
}


int cPacketHeader::GetProtocolId(const BYTE *src)
{
	return *(int*)src;
}

uint cPacketHeader::GetPacketId(const BYTE *src)
{
	return *(uint*)(src + sizeof(int));
}


uint cPacketHeader::GetPacketLength(const BYTE *src)
{
	return *(uint*)(src + sizeof(int) + sizeof(int));
}


void cPacketHeader::SetProtocolId(BYTE *dst, const int protocolId)
{
	*(int*)dst = protocolId;
}


void cPacketHeader::SetPacketId(BYTE *dst, const uint packetId)
{
	*(uint*)(dst + sizeof(int)) = packetId;
}


void cPacketHeader::SetPacketLength(BYTE *dst, const uint packetLength)
{
	*(short*)(dst + sizeof(int) + sizeof(int)) = (short)packetLength;
}


bool cPacketHeader::IsValidPacket(const BYTE *src)
{
	return (GetProtocolId(src) >= 0)
		&& (GetPacketId(src) >= 0)
		&& (GetPacketLength(src) > 0);
	return true;
}
