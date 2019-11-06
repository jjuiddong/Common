//------------------------------------------------------------------------
// Name:    remotedbg_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace remotedbg {

using namespace network2;
using namespace marshalling;
static const int s2c_Protocol_ID = 5200;

class s2c_Protocol : public network2::iProtocol
{
public:
	s2c_Protocol() : iProtocol(s2c_Protocol_ID) {}
	void AckOneStep(netid targetId);
	void AckDebugRun(netid targetId);
	void AckBreak(netid targetId);
	void AckTerminate(netid targetId);
	void UpdateInformation(netid targetId, const string &fileName, const string &vmName, const int &instIndex);
	void UpdateState(netid targetId, const string &vmName, const int &instIndex);
	void UpdateSymbolTable(netid targetId, const int &start, const int &count, const string &vmName, const vector<string> &symbols, const vector<int> &symbolVals);
};
static const int c2s_Protocol_ID = 5201;

class c2s_Protocol : public network2::iProtocol
{
public:
	c2s_Protocol() : iProtocol(c2s_Protocol_ID) {}
	void ReqOneStep(netid targetId);
	void ReqDebugRun(netid targetId);
	void ReqBreak(netid targetId);
	void ReqTerminate(netid targetId);
};
}
