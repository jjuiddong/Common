
#include "stdafx.h"
#include "protocolutil.h"

namespace network2
{
	bool Init();
	void InsertProtocol(sProtocol *protocol);
	void InsertPacket(sProtocol *protocol, sPacket *packet);
	sPacket* GetPacket(const __int64 packetId);

	map<__int64, sPacket*> g_packets; // key: protocolid << 32 + packetID
}

using namespace network2;


// Insert Protocol
void network2::InsertProtocol(sProtocol *protocol)
{
	RET(!protocol);
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


// Packet 이름을 리턴한다.
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


// g_packets 변수 제거
// cProtocolDisplayer 객체를 사용했다면, 프로그램이 종료 될 때, 
// 이 함수를 호출해서 파싱한 데이타를 제거해야 한다.
void network2::DisplayPacketCleanup()
{
	for (auto &prt : g_packets)
		ReleaseCurrentPacket(prt.second);
	g_packets.clear();
}
