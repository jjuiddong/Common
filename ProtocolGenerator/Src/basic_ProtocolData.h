//------------------------------------------------------------------------
// Name:    basic_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace basic {

using namespace network2;
using namespace marshalling;


	struct func1_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct func2_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string str;
	};

	struct func3_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		float value;
	};

	struct func4_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct func5_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		vector<int> ar;
	};

	struct func6_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		vector<int> ar;
	};





	struct func5_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		std::string str;
	};

	struct func6_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		float value;
	};



}
