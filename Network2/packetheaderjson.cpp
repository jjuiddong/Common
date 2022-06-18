#include "stdafx.h"
#include "packetheaderjson.h"
#include "marshalling_json.h"

using namespace network2;


// return packet header size
// protocol ID + packet ID + packet Length + option
uint cPacketHeaderJson::GetHeaderSize()
{
	return sizeof(int) * 4; // 16 bytes
}


int cPacketHeaderJson::GetProtocolId(const BYTE *src)
{
	return *(int*)src;
}

uint cPacketHeaderJson::GetPacketId(const BYTE *src)
{
	return *(uint*)(src + sizeof(int));
}


uint cPacketHeaderJson::GetPacketLength(const BYTE *src)
{
	return *(uint*)(src + sizeof(int) + sizeof(int));
}


void cPacketHeaderJson::SetProtocolId(BYTE *dst, const int protocolId)
{
	*(int*)dst = protocolId;
}


void cPacketHeaderJson::SetPacketId(BYTE *dst, const uint packetId)
{
	*(uint*)(dst + sizeof(int)) = packetId;
}


void cPacketHeaderJson::SetPacketLength(BYTE *dst, const uint packetLength)
{
	*(uint*)(dst + sizeof(int) + sizeof(int)) = packetLength;
}


bool cPacketHeaderJson::IsValidPacket(const BYTE *src)
{
	return (GetProtocolId(src) >= 0)
		//&& (GetPacketId(src) >= 0)
		//&& (GetPacketLength(src) > 0)
		;
	return true;
}


uint cPacketHeaderJson::GetOptionBits(const BYTE *src, const uint mask)
{
	return *(uint*)(src + (sizeof(int) * 3)) & mask;
}


void cPacketHeaderJson::SetOptionBits(BYTE *dst, const uint mask, const uint options)
{
	*(uint*)(dst + (sizeof(int) * 3)) = 0; // clear option bit
	*(uint*)(dst + (sizeof(int) * 3)) = 
		(*(uint*)(dst + (sizeof(int) * 3)) & ~mask)
		| (mask & options);
}
