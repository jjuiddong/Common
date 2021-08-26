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

	struct UploadIntermediateCode_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		string code;
	};

	struct ReqIntermediateCode_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
	};

	struct ReqRun_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		string runType;
	};

	struct ReqOneStep_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
	};

	struct ReqResumeRun_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
	};

	struct ReqBreak_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
	};

	struct ReqBreakPoint_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		bool enable;
		uint id;
	};

	struct ReqStop_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
	};

	struct ReqInput_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmIdx;
		string eventName;
	};

	struct ReqEvent_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmIdx;
		string eventName;
	};

	struct ReqStepDebugType_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int stepDbgType;
	};

	struct ReqHeartBeat_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};





	struct AckUploadIntermediateCode_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int result;
	};

	struct AckIntermediateCode_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int result;
		uint count;
		uint index;
		vector<BYTE> data;
	};

	struct AckRun_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int result;
	};

	struct AckOneStep_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int result;
	};

	struct AckResumeRun_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int result;
	};

	struct AckBreak_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int result;
	};

	struct AckBreakPoint_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		bool enable;
		uint id;
		int result;
	};

	struct AckStop_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int result;
	};

	struct AckInput_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int result;
	};

	struct AckEvent_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmIdx;
		string eventName;
		int result;
	};

	struct AckStepDebugType_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int stepDbgType;
		int result;
	};

	struct SyncVMInstruction_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmIdx;
		vector<ushort> indices;
	};

	struct SyncVMRegister_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmIdx;
		int infoType;
		script::cVirtualMachine::sRegister reg;
	};

	struct SyncVMSymbolTable_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmIdx;
		uint start;
		uint count;
		vector<script::sSyncSymbol> symbol;
	};

	struct SyncVMOutput_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmIdx;
		string output;
	};

	struct AckHeartBeat_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};



}
