//------------------------------------------------------------------------
// Name:    test_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace test {

using namespace network2;
using namespace marshalling_json;


	struct AckResult_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string packetName;
		float result;
		map<string,vector<string>> vars;
	};





	struct PacketName1_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string id;
		string data;
		float num;
	};



}
