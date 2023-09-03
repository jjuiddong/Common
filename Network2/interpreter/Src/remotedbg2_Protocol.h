//------------------------------------------------------------------------
// Name:    remotedbg2_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace remotedbg2 {

using namespace network2;
using namespace marshalling_json;
static const int r2h_Protocol_ID = 5301;

class r2h_Protocol : public network2::iProtocol
{
public:
	r2h_Protocol() : iProtocol(r2h_Protocol_ID, ePacketFormat::JSON) {}
	void UploadIntermediateCode(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &code);
	void ReqIntermediateCode(netid targetId, bool isBinary, const int &itprId, const int &vmId);
	void ReqRun(netid targetId, bool isBinary, const int &itprId, const string &runType);
	void ReqOneStep(netid targetId, bool isBinary, const int &itprId);
	void ReqResumeRun(netid targetId, bool isBinary, const int &itprId);
	void ReqBreak(netid targetId, bool isBinary, const int &itprId, const int &vmId);
	void ReqBreakPoint(netid targetId, bool isBinary, const int &itprId, const int &vmId, const bool &enable, const uint &id);
	void ReqStop(netid targetId, bool isBinary, const int &itprId);
	void ReqInput(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &eventName);
	void ReqEvent(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &eventName, const map<string,vector<string>> &values);
	void ReqStepDebugType(netid targetId, bool isBinary, const int &stepDbgType);
	void ReqDebugInfo(netid targetId, bool isBinary, const vector<int> &vmIds);
	void ReqVariableInfo(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &varName);
	void ReqChangeVariable(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &varName, const string &value);
	void ReqHeartBeat(netid targetId, bool isBinary);
	static cPacketHeaderJson s_packetHeader;
};
static const int h2r_Protocol_ID = 5300;

class h2r_Protocol : public network2::iProtocol
{
public:
	h2r_Protocol() : iProtocol(h2r_Protocol_ID, ePacketFormat::JSON) {}
	void Welcome(netid targetId, bool isBinary, const string &msg);
	void AckUploadIntermediateCode(netid targetId, bool isBinary, const int &itprId, const int &vmId, const int &result);
	void AckIntermediateCode(netid targetId, bool isBinary, const BYTE &itprId, const int &vmId, const BYTE &result, const BYTE &count, const BYTE &index, const uint &totalBufferSize, const vector<BYTE> &data);
	void SpawnTotalInterpreterInfo(netid targetId, bool isBinary, const int &totalCount, const int &index, const int &itprId, const vector<int> &vmIds);
	void SpawnInterpreterInfo(netid targetId, bool isBinary, const int &itprId, const int &parentVmId, const int &vmId, const string &nodeFileName, const string &nodeName);
	void RemoveInterpreter(netid targetId, bool isBinary, const int &vmId);
	void AckRun(netid targetId, bool isBinary, const int &itprId, const int &result);
	void AckOneStep(netid targetId, bool isBinary, const int &itprId, const int &result);
	void AckResumeRun(netid targetId, bool isBinary, const int &itprId, const int &result);
	void AckBreak(netid targetId, bool isBinary, const int &itprId, const int &vmId, const int &result);
	void AckBreakPoint(netid targetId, bool isBinary, const int &itprId, const int &vmId, const bool &enable, const uint &id, const int &result);
	void AckStop(netid targetId, bool isBinary, const int &itprId, const int &result);
	void AckInput(netid targetId, bool isBinary, const int &itprId, const int &vmId, const int &result);
	void AckEvent(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &eventName, const int &result);
	void AckStepDebugType(netid targetId, bool isBinary, const int &stepDbgType, const int &result);
	void AckDebugInfo(netid targetId, bool isBinary, const vector<int> &vmIds, const int &result);
	void AckChangeVariable(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &varName, const int &result);
	void SyncVMInstruction(netid targetId, bool isBinary, const int &itprId, const int &vmId, const vector<ushort> &indices);
	void SyncVMRegister(netid targetId, bool isBinary, const int &itprId, const int &vmId, const int &infoType, const script::cVirtualMachine::sRegister &reg);
	void SyncVMSymbolTable(netid targetId, bool isBinary, const int &itprId, const int &vmId, const uint &start, const uint &count, const vector<script::sSyncSymbol> &symbol);
	void SyncVMOutput(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &output);
	void SyncVMWidgets(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &widgetName);
	void SyncVMArray(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &varName, const uint &startIdx, const vector<variant_t> &array);
	void SyncVMArrayBool(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &varName, const uint &startIdx, const vector<bool> &array);
	void SyncVMArrayNumber(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &varName, const uint &startIdx, const vector<float> &array);
	void SyncVMArrayString(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &varName, const uint &startIdx, const vector<string> &array);
	void SyncVMTimer(netid targetId, bool isBinary, const int &itprId, const int &vmId, const string &scopeName, const int &timerId, const int &time, const int &actionType);
	void AckHeartBeat(netid targetId, bool isBinary);
	static cPacketHeaderJson s_packetHeader;
};
}
