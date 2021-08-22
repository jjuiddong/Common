//------------------------------------------------------------------------
// Name:    remotedbg2_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace remotedbg2 {

using namespace network2;
using namespace marshalling_json;
static const int h2r_Protocol_ID = 5300;

class h2r_Protocol : public network2::iProtocol
{
public:
	h2r_Protocol() : iProtocol(h2r_Protocol_ID, ePacketFormat::JSON) {}
	void AckUploadIntermediateCode(netid targetId, bool isBinary, const int &itprId, const int &result);
	void AckIntermediateCode(netid targetId, bool isBinary, const int &itprId, const int &result, const uint &count, const uint &index, const vector<BYTE> &data);
	void AckRun(netid targetId, bool isBinary, const int &itprId, const int &result);
	void AckOneStep(netid targetId, bool isBinary, const int &itprId, const int &result);
	void AckResumeRun(netid targetId, bool isBinary, const int &itprId, const int &result);
	void AckBreak(netid targetId, bool isBinary, const int &itprId, const int &result);
	void AckBreakPoint(netid targetId, bool isBinary, const int &itprId, const bool &enable, const uint &id, const int &result);
	void AckStop(netid targetId, bool isBinary, const int &itprId, const int &result);
	void AckInput(netid targetId, bool isBinary, const int &itprId, const int &result);
	void AckStepDebugType(netid targetId, bool isBinary, const int &stepDbgType, const int &result);
	void SyncVMInstruction(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const vector<ushort> &indices);
	void SyncVMRegister(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const int &infoType, const script::cVirtualMachine::sRegister &reg);
	void SyncVMSymbolTable(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const uint &start, const uint &count, const vector<script::sSyncSymbol> &symbol);
	void SyncVMOutput(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const string &output);
	void AckHeartBeat(netid targetId, bool isBinary);
};
}
