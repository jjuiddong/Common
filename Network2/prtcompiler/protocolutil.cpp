
#include "stdafx.h"
#include "protocolutil.h"

namespace network2
{
	bool Init();
	void InsertRmi(sRmi *pRmi);
	void InsertProtocol(sRmi *pRmi, sProtocol *pProtocol);
	sProtocol* GetProtocol(const __int64 packetId);

	map<__int64, sProtocol*> g_Protocols; // key: protocolid << 32 + packetID
}

using namespace network2;


// Insert Rmi
void network2::InsertRmi(sRmi *pRmi)
{
	RET(!pRmi);
	InsertProtocol(pRmi, pRmi->protocol);
	InsertRmi(pRmi->next);
}


// Insert  Protocol
void network2::InsertProtocol(sRmi *pRmi, sProtocol *pProtocol)
{
	RET(!pProtocol);
	const __int64 id = ((__int64)pRmi->number << 32) + pProtocol->packetId;
	g_Protocols.insert({ id, pProtocol });
	InsertProtocol(pRmi, pProtocol->next);
}


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

		sRmi *rmiList = parser.Parse(str.c_str(), FALSE, FALSE);
		InsertRmi(rmiList);
		ReleaseRmiOnly(rmiList);
	}

	return true;
}


// return Protocol Parse Tree
sProtocol* network2::GetProtocol(const __int64 packetId)
{
	auto it = g_Protocols.find(packetId);
	if (g_Protocols.end() == it)
		return NULL;
	return it->second;
}


void network2::DisplayPacket(const Str128 &firstStr, const cPacket &packet
	, const int logLevel //= 0
)
{
	if (g_Protocols.empty())
		Init();

	const int protocolID = packet.GetProtocolId();
	const uint packetID = packet.GetPacketId();	
	const __int64 id = ((__int64)protocolID << 32) + packetID;
	sProtocol *protocol = GetProtocol(id);
	std::stringstream ss;
	ss << firstStr.c_str();
	ss << Packet2String(packet, protocol);
	ss << std::endl;
	common::dbg::Logc(logLevel, ss.str().c_str());
}


// Packet정보를 문자열로 변환해 리턴한다.
void network2::GetPacketString(const cPacket &packet, OUT string &out)
{
	if (g_Protocols.empty())
		Init();

	const int protocolID = packet.GetProtocolId();
	const uint packetID = packet.GetPacketId();
	const __int64 id = ((__int64)protocolID << 32) + packetID;
	sProtocol *protocol = GetProtocol(id);
	out = Packet2String(packet, protocol);
}


// Packet 이름을 리턴한다.
StrId network2::GetPacketName(const cPacket &packet)
{
	if (g_Protocols.empty())
		Init();

	const int protocolID = packet.GetProtocolId();
	const uint packetID = packet.GetPacketId();
	const __int64 id = ((__int64)protocolID << 32) + packetID;
	sProtocol *protocol = GetProtocol(id);
	if (!protocol)
		return "Unknown Packet";
	return protocol->name;
}


// g_Protocols 변수 제거
// cProtocolDisplayer 객체를 사용했다면, 프로그램이 종료 될 때, 
// 이 함수를 호출해서 파싱한 데이타를 제거해야 한다.
void network2::DisplayPacketCleanup()
{
	for (auto &prt : g_Protocols)
		ReleaseCurrentProtocol(prt.second);
	g_Protocols.clear();
}
