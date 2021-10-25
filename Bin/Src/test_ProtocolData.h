//------------------------------------------------------------------------
// Name:    test_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace test {

using namespace network2;
using namespace marshalling;


	struct AckResult_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string packetName;
		float result;
		map<string,string> vars;
	};





	struct PacketName1_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string id;
		string data;
		float num;
	};



}
