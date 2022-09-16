#include "pch.h"
#include "test_Protocol.h"
using namespace test;

cPacketHeaderJson test::s2c_Protocol::s_packetHeader;
//------------------------------------------------------------------------
// Protocol: AckResult
//------------------------------------------------------------------------
void test::s2c_Protocol::AckResult(netid targetId, bool isBinary, const string &packetName, const float &result, const map<string,vector<string>> &vars)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3846277350 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, packetName);
		marshalling::operator<<(packet, result);
		marshalling::operator<<(packet, vars);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "packetName", packetName);
			put(props, "result", result);
			put(props, "vars", vars);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}



cPacketHeaderJson test::c2s_Protocol::s_packetHeader;
//------------------------------------------------------------------------
// Protocol: PacketName1
//------------------------------------------------------------------------
void test::c2s_Protocol::PacketName1(netid targetId, bool isBinary, const string &id, const string &data, const float &num)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2900479863 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, id);
		marshalling::operator<<(packet, data);
		marshalling::operator<<(packet, num);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "id", id);
			put(props, "data", data);
			put(props, "num", num);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}



