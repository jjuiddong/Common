//------------------------------------------------------------------------
// Name:    remotedbg2_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace remotedbg2 {

using namespace network2;
using namespace marshalling_json;


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
