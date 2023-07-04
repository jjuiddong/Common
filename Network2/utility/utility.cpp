
#include "stdafx.h"
#include "utility.h"

namespace network2
{
	// *.prt file parsing functions
	void InsertProtocol(sProtocol *protocol);
	void InsertPacket(sProtocol *protocol, sPacket *packet);
	sPacket* GetPacket(const __int64 packetId);
	//~

	std::atomic<bool> g_isLoadProtocol = false; // load *.prt file?
	CriticalSection g_cs; // sync initialize g_packets, g_protocolFormat
	map<int64, sPacket*> g_packets; // key: protocolid << 32 + packetID
	map<int, ePacketFormat> g_protocolFormat; // key: protocolId
}

using namespace network2;


// packet data display task
class cPacketDisplayTask : public common::cTask
						 , public common::cMemoryPool4<cPacketDisplayTask>
{
public:
	cPacket m_packet;
	int m_logLevel;
	string m_prefix;
	cPacketDisplayTask(const Str128 &prefix, const cPacket &packet, const int logLevel = 0)
		: common::cTask(0, "cPacketDisplayTask") 
		, m_packet(packet)
		, m_logLevel(logLevel)
		, m_prefix(prefix.c_str())
	{
	}

	virtual eRunResult Run(const double deltaSeconds) 
	{
		const int protocolID = m_packet.GetProtocolId();
		const uint packetID = m_packet.GetPacketId();
		const __int64 id = ((__int64)protocolID << 32) + packetID;
		sPacket *p = GetPacket(id);
		std::stringstream ss;
		ss << m_prefix;
		ss << Packet2String(m_packet, p);
		ss << std::endl;
		common::dbg::Logc(m_logLevel, ss.str().c_str());
		return eRunResult::End; 
	}
};
//~


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
bool network2::InitPacketParser()
{
	AutoCSLock cs(g_cs);
	if (g_isLoadProtocol)
		return true; // already loaded

	list<string> exts;
	exts.push_back(".prt");
	string protocolDir = "./media/protocol/";
	list<string> fileList;
	common::CollectFiles(exts, protocolDir, fileList);
	for (auto &str : fileList)
	{
		cProtocolParser parser;
		parser.SetAutoRemove(false);

		sStmt *stmts = parser.Parse(str.c_str(), false, false);
		if (stmts)
			InsertProtocol(stmts->protocol);
		ReleaseProtocolOnly(stmts);
	}
	g_isLoadProtocol = true;
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


// write packet string data to logfile
void network2::DisplayPacket(const string &firstStr, const cPacket &packet
	, const int logLevel //= 0
)
{
	if (!g_isLoadProtocol)
		InitPacketParser();

	network2::GetLogThread().PushTask(new cPacketDisplayTask(firstStr, packet, logLevel));

	//const int protocolID = packet.GetProtocolId();
	//const uint packetID = packet.GetPacketId();
	//const __int64 id = ((__int64)protocolID << 32) + packetID;
	//sPacket *p = GetPacket(id);
	//std::stringstream ss;
	//ss << firstStr.c_str();
	//ss << Packet2String(packet, p);
	//ss << std::endl;
	//common::dbg::Logc(logLevel, ss.str().c_str());
}


// convert packet data to string (for debugging)
void network2::GetPacketString(const cPacket &packet, OUT string &out)
{
	if (!g_isLoadProtocol)
		InitPacketParser();

	const int protocolID = packet.GetProtocolId();
	const uint packetID = packet.GetPacketId();
	const __int64 id = ((__int64)protocolID << 32) + packetID;
	sPacket *p = GetPacket(id);
	out = Packet2String(packet, p);
}


// return packet name
StrId network2::GetPacketName(const cPacket &packet)
{
	if (!g_isLoadProtocol)
		InitPacketParser();

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
	if (!g_isLoadProtocol)
		InitPacketParser();
	if (0 == protocolId) // basic protocol?
		return ePacketFormat::BINARY;
	auto it = g_protocolFormat.find(protocolId);
	if (g_protocolFormat.end() == it)
		return ePacketFormat::FREE; // exception 
	return it->second;
}


// remove global packet data especially g_packets
// call this function when terminate program
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
	// global(static variable), packet header
	static cPacketHeader s_packetHeaderBinary;
	static cPacketHeaderAscii s_packetHeaderAscii;
	static cPacketHeaderNoFormat s_packetHeaderNoFormat;

	auto it = cProtocolDispatcher::GetPacketHeaderMap()->find(protocolId);
	if (cProtocolDispatcher::GetPacketHeaderMap()->end() != it)
		return it->second;

	// basic protocol?
	if (0 == protocolId)
		return &s_packetHeaderBinary;

	// check ascii format, alphabet A~Z
	const char code = (protocolId & 0xff000000) >> 24;
	if ((code >= 65) && (code <= 90))
		return &s_packetHeaderAscii;

	// get packet header from media/protocol/*.prt files
	const ePacketFormat format = GetPacketFormat(protocolId);
	return GetPacketHeader(format);
}


// return global packet header correspond packet format
iPacketHeader* network2::GetPacketHeader(const ePacketFormat format)
{
	// global (static variable), packet header
	static cPacketHeader s_packetHeaderBinary;
	static cPacketHeaderAscii s_packetHeaderAscii;
	static cPacketHeaderJson s_packetHeaderJson;
	static cPacketHeaderNoFormat s_packetHeaderNoFormat;

	switch (format)
	{
	case ePacketFormat::BINARY: return &s_packetHeaderBinary;
	case ePacketFormat::JSON: return &s_packetHeaderJson;
	case ePacketFormat::ASCII: return &s_packetHeaderAscii;
	case ePacketFormat::FREE: 
	default: return &s_packetHeaderNoFormat;
	}
}
