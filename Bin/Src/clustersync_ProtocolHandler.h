//------------------------------------------------------------------------
// Name:    clustersync_ProtocolHandler.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "clustersync_ProtocolData.h"

namespace clustersync {

using namespace network2;
using namespace marshalling_json;
static const int s2c_Dispatcher_ID = 8000;

// Protocol Dispatcher
class s2c_Dispatcher: public network2::cProtocolDispatcher
{
public:
	s2c_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static s2c_Dispatcher g_clustersync_s2c_Dispatcher;


// ProtocolHandler
class s2c_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class s2c_Dispatcher;
	s2c_ProtocolHandler() { m_format = ePacketFormat::JSON; }
};


static const int c2s_Dispatcher_ID = 8100;

// Protocol Dispatcher
class c2s_Dispatcher: public network2::cProtocolDispatcher
{
public:
	c2s_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static c2s_Dispatcher g_clustersync_c2s_Dispatcher;


// ProtocolHandler
class c2s_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class c2s_Dispatcher;
	c2s_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool SpawnRobot(clustersync::SpawnRobot_Packet &packet) { return true; }
	virtual bool RemoveRobot(clustersync::RemoveRobot_Packet &packet) { return true; }
	virtual bool RobotWorkFinish(clustersync::RobotWorkFinish_Packet &packet) { return true; }
	virtual bool RealtimeRobotState(clustersync::RealtimeRobotState_Packet &packet) { return true; }
};


}
