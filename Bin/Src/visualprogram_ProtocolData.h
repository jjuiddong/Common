//------------------------------------------------------------------------
// Name:    visualprogram_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace visualprogram {

using namespace network2;
using namespace marshalling_json;


	struct AckWebLogin_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string id;
		int result;
	};

	struct AckWebRunVisualProgStream_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string nodeFileName;
		int result;
	};

	struct AckWebStopVisualProg_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct ReqWebConnectRIProxyServer_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string url;
		string name;
	};





	struct ReqWebLogin_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string id;
	};

	struct ReqWebRunVisualProgStream_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string nodeFileName;
	};

	struct ReqWebStopVisualProg_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};





	struct Welcome_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string msg;
	};

	struct AckLogin_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string id;
		int result;
	};

	struct ReqRunVisualProg_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		webvpl::sNodeFile nodeFile;
	};

	struct ReqRunVisualProgStream_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		uint count;
		uint index;
		vector<BYTE> data;
	};

	struct ReqRunVisualProgFiles_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		vector<string> nodeFileNames;
	};

	struct ReqStopVisualProg_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct ReqConnectRIProxyServer_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string url;
		string name;
	};





	struct ReqLogin_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string id;
	};

	struct AckRunVisualProg_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct AckRunVisualProgStream_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct AckRunVisualProgFiles_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct AckStopVisualProg_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct AckConnectRIProxyServer_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string url;
		string name;
		int result;
	};



}
