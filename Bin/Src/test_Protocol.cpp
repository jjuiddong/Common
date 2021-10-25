#include "pch.h"
#include "test_Protocol.h"
using namespace test;

cPacketHeader test::s2c_Protocol::s_packetHeader;
//------------------------------------------------------------------------
// Protocol: AckResult
//------------------------------------------------------------------------
void test::s2c_Protocol::AckResult(netid targetId, const string &packetName, const float &result, const map<string,string> &vars)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3846277350 );
	packet << packetName;
	packet << result;
	packet << vars;
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}



cPacketHeader test::c2s_Protocol::s_packetHeader;
//------------------------------------------------------------------------
// Protocol: PacketName1
//------------------------------------------------------------------------
void test::c2s_Protocol::PacketName1(netid targetId, const string &id, const string &data, const float &num)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2900479863 );
	packet << id;
	packet << data;
	packet << num;
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}



