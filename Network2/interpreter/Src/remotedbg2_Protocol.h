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
	void UploadVProgFile(netid targetId, bool isBinary, const string &code);
	void ReqIntermediateCode(netid targetId, bool isBinary);
	void ReqRun(netid targetId, bool isBinary);
	void ReqOneStep(netid targetId, bool isBinary);
	void ReqDebugRun(netid targetId, bool isBinary);
	void ReqBreak(netid targetId, bool isBinary);
	void ReqStop(netid targetId, bool isBinary);
	void ReqInput(netid targetId, bool isBinary, const int &vmIdx, const string &eventName);
};
static const int h2r_Protocol_ID = 5300;

class h2r_Protocol : public network2::iProtocol
{
public:
	h2r_Protocol() : iProtocol(h2r_Protocol_ID, ePacketFormat::JSON) {}
	void AckUploadVProgFile(netid targetId, bool isBinary, const int &result);
	void AckIntermediateCode(netid targetId, bool isBinary, const int &result, const script::cIntermediateCode &code);
	void AckRun(netid targetId, bool isBinary, const int &result);
	void AckOneStep(netid targetId, bool isBinary, const int &result);
	void AckDebugRun(netid targetId, bool isBinary, const int &result);
	void AckBreak(netid targetId, bool isBinary, const int &result);
	void AckStop(netid targetId, bool isBinary, const int &result);
	void AckInput(netid targetId, bool isBinary, const int &result);
	void SyncVMInstruction(netid targetId, bool isBinary, const int &vmIdx, const uint &index, const bool &cmp);
	void SyncVMRegister(netid targetId, bool isBinary, const int &vmIdx, const int &infoType, const script::cVirtualMachine::sRegister &reg);
	void SyncVMSymbolTable(netid targetId, bool isBinary, const int &vmIdx, const int &start, const int &count, const string &symbol);
	void SyncOutput(netid targetId, bool isBinary, const int &vmIdx, const string &output);
};
}
