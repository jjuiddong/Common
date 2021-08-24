//------------------------------------------------------------------------
// Name:    editor_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace editor {

using namespace network2;
using namespace marshalling_json;
static const int s2c_Protocol_ID = 9000;

class s2c_Protocol : public network2::iProtocol
{
public:
	s2c_Protocol() : iProtocol(s2c_Protocol_ID, ePacketFormat::JSON) {}
	void AckRunSimulation(netid targetId, bool isBinary, const string &projectName, const string &mapFileName, const string &pathFileName, const vector<string> &nodeFileNames, const string &taskName, const int &result);
	void AckRunProject(netid targetId, bool isBinary, const string &projectName, const int &result);
	void AckStopSimulation(netid targetId, bool isBinary, const int &result);
};
static const int c2s_Protocol_ID = 9100;

class c2s_Protocol : public network2::iProtocol
{
public:
	c2s_Protocol() : iProtocol(c2s_Protocol_ID, ePacketFormat::JSON) {}
	void ReqRunSimulation(netid targetId, bool isBinary, const string &projectName, const string &mapFileName, const string &pathFileName, const vector<string> &nodeFileNames, const string &taskName);
	void ReqRunProject(netid targetId, bool isBinary, const string &projectName);
	void ReqStopSimulation(netid targetId, bool isBinary);
};
}
