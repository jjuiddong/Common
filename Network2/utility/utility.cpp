
#include "stdafx.h"
#include "utility.h"

using namespace network2;

namespace network2
{
	bool Init();
	void InsertProtocol(sProtocol *protocol);
	void InsertPacket(sProtocol *protocol, sPacket *packet);
	sPacket* GetPacket(const __int64 packetId);

	map<__int64, sPacket*> g_packets; // key: protocolid << 32 + packetID
	map<__int64, ePacketFormat> g_protocolFormat; // key: protocolid
}


// Insert Protocol
void network2::InsertProtocol(sProtocol *protocol)
{
	RET(!protocol);

	// update protocol format
	const ePacketFormat format = (protocol->format == "json") ?
		ePacketFormat::JSON : ((protocol->format == "ascii") ?
			ePacketFormat::ASCII : ePacketFormat::BINARY);
	g_protocolFormat.insert({ protocol->number, format });
	//~

	InsertPacket(protocol, protocol->packet);
	InsertProtocol(protocol->next);
}


// Insert Packet
void network2::InsertPacket(sProtocol *protocol, sPacket *packet)
{
	RET(!packet);
	const __int64 id = ((__int64)protocol->number << 32) + packet->packetId;
	g_packets.insert({ id, packet });
	InsertPacket(protocol, packet->next);
}


// read all *.prt file
// search directory "./media/protocol/*.prt"
bool network2::Init()
{
	list<string> exts;
	exts.push_back(".prt");
	string protocolDir = "./media/protocol/";
	list<string> fileList;
	common::CollectFiles(exts, protocolDir, fileList);
	for (auto &str : fileList)
	{
		cProtocolParser parser;
		parser.SetAutoRemove(FALSE);

		sStmt *stmts = parser.Parse(str.c_str(), FALSE, FALSE);
		if (stmts)
			InsertProtocol(stmts->protocol);
		ReleaseProtocolOnly(stmts);
	}
	return true;
}


// return Protocol Parse Tree
sPacket* network2::GetPacket(const __int64 packetId)
{
	auto it = g_packets.find(packetId);
	if (g_packets.end() == it)
		return NULL;
	return it->second;
}


// display packet data
void network2::DisplayPacket(const Str128 &firstStr, const cPacket &packet
	, const int logLevel //= 0
)
{
	if (g_packets.empty())
		Init();

	const int protocolID = packet.GetProtocolId();
	const uint packetID = packet.GetPacketId();
	const __int64 id = ((__int64)protocolID << 32) + packetID;
	sPacket *p = GetPacket(id);
	std::stringstream ss;
	ss << firstStr.c_str();
	ss << Packet2String(packet, p);
	ss << std::endl;
	common::dbg::Logc(logLevel, ss.str().c_str());
}


// Packet정보를 문자열로 변환해 리턴한다.
void network2::GetPacketString(const cPacket &packet, OUT string &out)
{
	if (g_packets.empty())
		Init();

	const int protocolID = packet.GetProtocolId();
	const uint packetID = packet.GetPacketId();
	const __int64 id = ((__int64)protocolID << 32) + packetID;
	sPacket *p = GetPacket(id);
	out = Packet2String(packet, p);
}


// return packet name
StrId network2::GetPacketName(const cPacket &packet)
{
	if (g_packets.empty())
		Init();

	const int protocolID = packet.GetProtocolId();
	const uint packetID = packet.GetPacketId();
	const __int64 id = ((__int64)protocolID << 32) + packetID;
	sPacket *p = GetPacket(id);
	if (!p)
		return "Unknown Packet";
	return p->name;
}


// return packet format, binary, ascii, json
// find from *.prt file
ePacketFormat network2::GetPacketFormat(const cPacket &packet)
{
	const int protocolID = packet.GetProtocolId();
	return GetPacketFormat(protocolID);
}


// return packet format by protocol id
// find from *.prt file
ePacketFormat network2::GetPacketFormat(const int protocolId)
{
	if (g_packets.empty())
		Init();
	// basic protocol?
	if (0 == protocolId)
		return ePacketFormat::BINARY;
	auto it = g_protocolFormat.find(protocolId);
	if (g_protocolFormat.end() == it)
		return ePacketFormat::FREE; // exception 
	return it->second;
}


// g_packets 변수 제거
// cProtocolDisplayer 객체를 사용했다면, 프로그램이 종료 될 때, 
// 이 함수를 호출해서 파싱한 데이타를 제거해야 한다.
void network2::DisplayPacketCleanup()
{
	for (auto &prt : g_packets)
		ReleaseCurrentPacket(prt.second);
	g_packets.clear();
	g_protocolFormat.clear();
}


// get packet header by protocol id
// find from cProtocolDispatcher::GetPacketHeaderMap()
iPacketHeader* network2::GetPacketHeader(const int protocolId)
{
	// global packet header
	static cPacketHeader s_packetHeader; // binary packet header
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

	const ePacketFormat format = GetPacketFormat(protocolId);
	return GetPacketHeader(format);
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
