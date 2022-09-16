//------------------------------------------------------------------------
// Name:    test_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace test {

using namespace network2;
using namespace marshalling_json;
static const int s2c_Protocol_ID = 100000;

class s2c_Protocol : public network2::iProtocol
{
public:
	s2c_Protocol() : iProtocol(s2c_Protocol_ID, ePacketFormat::JSON) {}
	void AckResult(netid targetId, bool isBinary, const string &packetName, const float &result, const map<string,vector<string>> &vars);
	static cPacketHeaderJson s_packetHeader;
};
static const int c2s_Protocol_ID = 100001;

class c2s_Protocol : public network2::iProtocol
{
public:
	c2s_Protocol() : iProtocol(c2s_Protocol_ID, ePacketFormat::JSON) {}
	void PacketName1(netid targetId, bool isBinary, const string &id, const string &data, const float &num);
	static cPacketHeaderJson s_packetHeader;
};
}
