
#include "stdafx.h"
#include "utility.h"

using namespace network2;


// get packet header by protocol id
iPacketHeader* network2::GetPacketHeader(const int protocolId)
{
	// exception packet header
	static cPacketHeader s_packetHeader;
	static cPacketHeaderAscii s_packetHeaderAscii;
	static cPacketHeaderNoFormat s_packetHeaderNoFormat;

	auto it = cProtocolDispatcher::GetPacketHeaderMap()->find(protocolId);
	if (cProtocolDispatcher::GetPacketHeaderMap()->end() != it)
		return it->second;

	// basic protocol?
	if (0 == protocolId)
		return &s_packetHeader;

	// check ascii format, alphabet A~Z
	const char code = (protocolId & 0xff000000) >> 24;
	if ((code >= 65) && (code <= 90))
		return &s_packetHeaderAscii;
	return &s_packetHeaderNoFormat;
}


// return global packet header correspond packet format
iPacketHeader* network2::GetPacketHeader(const ePacketFormat format)
{
	// global packet header
	static cPacketHeader s_packetHeader;
	static cPacketHeaderAscii s_packetHeaderAscii;
	static cPacketHeaderJson s_packetHeaderJson;
	static cPacketHeaderNoFormat s_packetHeaderNoFormat;

	switch (format)
	{
	case ePacketFormat::BINARY: return &s_packetHeader;
	case ePacketFormat::JSON: return &s_packetHeaderJson;
	case ePacketFormat::ASCII: return &s_packetHeaderAscii;
	case ePacketFormat::FREE: 
	default: return &s_packetHeaderNoFormat;
	}
}
