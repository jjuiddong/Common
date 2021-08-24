//------------------------------------------------------------------------
// Name:    editor_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace editor {

using namespace network2;
using namespace marshalling_json;


	struct AckRunSimulation_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string projectName;
		string mapFileName;
		string pathFileName;
		vector<string> nodeFileNames;
		string taskName;
		int result;
	};

	struct AckRunProject_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string projectName;
		int result;
	};

	struct AckStopSimulation_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};





	struct ReqRunSimulation_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string projectName;
		string mapFileName;
		string pathFileName;
		vector<string> nodeFileNames;
		string taskName;
	};

	struct ReqRunProject_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string projectName;
	};

	struct ReqStopSimulation_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};



}
