//------------------------------------------------------------------------
// Name:    remotedbg2_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace remotedbg2 {

using namespace network2;
using namespace marshalling_json;


	struct UploadVProgFile_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string code;
	};

	struct ReqIntermediateCode_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct ReqRun_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
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

	struct ReqStop_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct ReqInput_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int vmIdx;
		string eventName;
	};

	struct ReqHeartBeat_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};





	struct AckUploadVProgFile_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct AckIntermediateCode_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
		script::cIntermediateCode code;
	};

	struct AckRun_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct AckOneStep_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct AckDebugRun_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct AckBreak_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct AckStop_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct AckInput_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int result;
	};

	struct SyncVMInstruction_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int vmIdx;
		vector<uint> indices;
		vector<bool> cmps;
	};

	struct SyncVMRegister_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int vmIdx;
		int infoType;
		script::cVirtualMachine::sRegister reg;
	};

	struct SyncVMSymbolTable_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int vmIdx;
		uint start;
		uint count;
		vector<script::sSyncSymbol> symbol;
	};

	struct SyncVMOutput_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int vmIdx;
		string output;
	};

	struct AckHeartBeat_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};



}
