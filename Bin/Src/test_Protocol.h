//------------------------------------------------------------------------
// Name:    test_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace test {

using namespace network2;
using namespace marshalling;
static const int s2c_Protocol_ID = 100000;

class s2c_Protocol : public network2::iProtocol
{
public:
	s2c_Protocol() : iProtocol(s2c_Protocol_ID, ePacketFormat::BINARY) {}
	void AckResult(netid targetId, const string &packetName, const float &result, const map<string,string> &vars);
	static cPacketHeader s_packetHeader;
};
static const int c2s_Protocol_ID = 100001;

class c2s_Protocol : public network2::iProtocol
{
public:
	c2s_Protocol() : iProtocol(c2s_Protocol_ID, ePacketFormat::BINARY) {}
	void PacketName1(netid targetId, const string &id, const string &data, const float &num);
	static cPacketHeader s_packetHeader;
};
}
