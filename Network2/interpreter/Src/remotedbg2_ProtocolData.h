//------------------------------------------------------------------------
// Name:    remotedbg2_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace remotedbg2 {

using namespace network2;
using namespace marshalling_json;


	struct Welcome_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string msg;
	};

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
		string runType;
	};

	struct ReqOneStep_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};

	struct ReqResumeRun_Packet {
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
		uint count;
		uint index;
		vector<BYTE> data;
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

	struct AckResumeRun_Packet {
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
