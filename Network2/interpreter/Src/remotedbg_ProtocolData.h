//------------------------------------------------------------------------
// Name:    remotedbg_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace remotedbg {

using namespace network2;
using namespace marshalling;


	struct AckOneStep_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct AckDebugRun_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct AckBreak_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct AckTerminate_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct UpdateInformation_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string fileName;
		string vmName;
		int instIndex;
	};

	struct UpdateState_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string vmName;
		int instIndex;
	};

	struct UpdateSymbolTable_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int start;
		int count;
		string vmName;
		vector<string> symbols;
		vector<int> symbolVals;
	};





	struct ReqOneStep_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct ReqDebugRun_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct ReqBreak_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct ReqTerminate_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};



}
