//------------------------------------------------------------------------
// Name:    controlserver_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace controlserver {

using namespace network2;
using namespace marshalling_json;
static const int s2c_Protocol_ID = 6000;

class s2c_Protocol : public network2::iProtocol
{
public:
	s2c_Protocol() : iProtocol(s2c_Protocol_ID, ePacketFormat::JSON) {}
	void Welcome(netid targetId, bool isBinary, const string &msg);
	void AckLogin(netid targetId, bool isBinary, const string &name, const int &result);
	void AckLogout(netid targetId, bool isBinary, const string &name, const int &result);
	void ReqStartCluster(netid targetId, bool isBinary, const string &name, const string &url, const string &projectName, const string &mapFileName, const string &pathFileName, const string &taskName);
	void ReqCloseCluster(netid targetId, bool isBinary, const string &name);
};
static const int c2s_Protocol_ID = 6100;

class c2s_Protocol : public network2::iProtocol
{
public:
	c2s_Protocol() : iProtocol(c2s_Protocol_ID, ePacketFormat::JSON) {}
	void ReqLogin(netid targetId, bool isBinary, const string &name);
	void ReqLogout(netid targetId, bool isBinary, const string &name);
	void AckStartCluster(netid targetId, bool isBinary, const string &name, const string &url, const int &result);
	void AckCloseCluster(netid targetId, bool isBinary, const string &name, const int &result);
	void RealtimeRobotState(netid targetId, bool isBinary, const string &robotName, const BYTE &state1, const BYTE &state2, const BYTE &state3, const BYTE &state4, const Vector3 &pos);
};
}
