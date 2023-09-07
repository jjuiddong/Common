//------------------------------------------------------------------------
// Name:    remotedbg2_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace remotedbg2 {

using namespace network2;
using namespace marshalling_json;


	struct UploadIntermediateCode_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string code;
	};

	struct ReqIntermediateCode_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
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
		int vmId;
	};

	struct ReqBreakPoint_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
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
		int vmId;
		string eventName;
	};

	struct ReqEvent_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string eventName;
		map<string,vector<string>> values;
	};

	struct ReqStepDebugType_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int stepDbgType;
	};

	struct ReqDebugInfo_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		vector<int> vmIds;
	};

	struct ReqVariableInfo_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string varName;
	};

	struct ReqChangeVariable_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string varName;
		string value;
	};

	struct ReqHeartBeat_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};





	struct Welcome_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string msg;
	};

	struct AckUploadIntermediateCode_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		int result;
	};

	struct AckIntermediateCode_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		BYTE itprId;
		int vmId;
		BYTE result;
		BYTE count;
		BYTE index;
		uint totalBufferSize;
		vector<BYTE> data;
	};

	struct SpawnTotalInterpreterInfo_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int totalCount;
		int index;
		int itprId;
		vector<int> vmIds;
	};

	struct SpawnInterpreterInfo_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int parentVmId;
		int vmId;
		string nodeFileName;
		string nodeName;
	};

	struct RemoveInterpreter_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int vmId;
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
		int vmId;
		int result;
	};

	struct AckBreakPoint_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
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
		int vmId;
		int result;
	};

	struct AckEvent_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string eventName;
		int result;
	};

	struct AckStepDebugType_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int stepDbgType;
		int result;
	};

	struct AckDebugInfo_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		vector<int> vmIds;
		int result;
	};

	struct AckChangeVariable_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string varName;
		int result;
	};

	struct SyncVMInstruction_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		vector<ushort> indices;
	};

	struct SyncVMRegister_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		int infoType;
		script::cVirtualMachine::sRegister reg;
	};

	struct SyncVMSymbolTable_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		uint start;
		uint count;
		vector<script::sSyncSymbol> symbol;
	};

	struct SyncVMOutput_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string output;
	};

	struct SyncVMWidgets_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string widgetName;
	};

	struct SyncVMArray_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string varName;
		uint startIdx;
		vector<variant_t> array;
	};

	struct SyncVMArrayBool_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string varName;
		uint startIdx;
		vector<bool> array;
	};

	struct SyncVMArrayNumber_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string varName;
		uint startIdx;
		vector<float> array;
	};

	struct SyncVMArrayString_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string varName;
		uint startIdx;
		vector<string> array;
	};

	struct SyncVMTimer_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		int itprId;
		int vmId;
		string scopeName;
		int timerId;
		int time;
		int actionType;
	};

	struct ExecuteCustomFunction_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string fnName;
		map<string,vector<string>> args;
	};

	struct AckHeartBeat_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
	};



}
