//------------------------------------------------------------------------
// Name:    controlserver_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace controlserver {

using namespace network2;
using namespace marshalling_json;


	struct Welcome_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string msg;
	};

	struct AckLogin_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		int result;
	};

	struct AckLogout_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		int result;
	};

	struct ReqStartCluster_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		string url;
		string projectName;
		string mapFileName;
		string pathFileName;
		string taskName;
	};

	struct ReqCloseCluster_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
	};





	struct ReqLogin_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
	};

	struct ReqLogout_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
	};

	struct AckStartCluster_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		string url;
		int result;
	};

	struct AckCloseCluster_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string name;
		int result;
	};

	struct RealtimeRobotState_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string robotName;
		BYTE state1;
		BYTE state2;
		BYTE state3;
		BYTE state4;
		Vector3 pos;
	};



}
