//------------------------------------------------------------------------
// Name:    clustersync_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace clustersync {

using namespace network2;
using namespace marshalling_json;
static const int s2c_Protocol_ID = 8000;

class s2c_Protocol : public network2::iProtocol
{
public:
	s2c_Protocol() : iProtocol(s2c_Protocol_ID, ePacketFormat::JSON) {}
};
static const int c2s_Protocol_ID = 8100;

class c2s_Protocol : public network2::iProtocol
{
public:
	c2s_Protocol() : iProtocol(c2s_Protocol_ID, ePacketFormat::JSON) {}
	void SpawnRobot(netid targetId, bool isBinary, const string &robotName, const string &modelName);
	void RemoveRobot(netid targetId, bool isBinary, const string &robotName);
	void RobotWorkFinish(netid targetId, bool isBinary, const string &robotName, const uint &taskId, const uint &totalWorkCount, const uint &finishWorkCount);
	void RealtimeRobotState(netid targetId, bool isBinary, const string &robotName, const BYTE &state1, const BYTE &state2, const BYTE &state3, const BYTE &state4, const Vector3 &pos, const Vector3 &dir);
};
}
