//------------------------------------------------------------------------
// Name:    clustersync_ProtocolData.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace clustersync {

using namespace network2;
using namespace marshalling_json;






	struct SpawnRobot_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string robotName;
		string modelName;
	};

	struct RemoveRobot_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string robotName;
	};

	struct RobotWorkFinish_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string robotName;
		uint taskId;
		uint totalWorkCount;
		uint finishWorkCount;
	};

	struct RealtimeRobotState_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string robotName;
		BYTE state1;
		BYTE state2;
		BYTE state3;
		BYTE state4;
		Vector3 pos;
		Vector3 dir;
	};



}
