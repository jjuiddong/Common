//------------------------------------------------------------------------
// Name:    controlserver_ProtocolHandler.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "controlserver_ProtocolData.h"

namespace controlserver {

using namespace network2;
using namespace marshalling_json;
static const int s2c_Dispatcher_ID = 6000;

// Protocol Dispatcher
class s2c_Dispatcher: public network2::cProtocolDispatcher
{
public:
	s2c_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static s2c_Dispatcher g_controlserver_s2c_Dispatcher;


// ProtocolHandler
class s2c_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class s2c_Dispatcher;
	s2c_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool Welcome(controlserver::Welcome_Packet &packet) { return true; }
	virtual bool AckLogin(controlserver::AckLogin_Packet &packet) { return true; }
	virtual bool AckLogout(controlserver::AckLogout_Packet &packet) { return true; }
	virtual bool ReqStartCluster(controlserver::ReqStartCluster_Packet &packet) { return true; }
	virtual bool ReqCloseCluster(controlserver::ReqCloseCluster_Packet &packet) { return true; }
};


static const int c2s_Dispatcher_ID = 6100;

// Protocol Dispatcher
class c2s_Dispatcher: public network2::cProtocolDispatcher
{
public:
	c2s_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static c2s_Dispatcher g_controlserver_c2s_Dispatcher;


// ProtocolHandler
class c2s_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class c2s_Dispatcher;
	c2s_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool ReqLogin(controlserver::ReqLogin_Packet &packet) { return true; }
	virtual bool ReqLogout(controlserver::ReqLogout_Packet &packet) { return true; }
	virtual bool AckStartCluster(controlserver::AckStartCluster_Packet &packet) { return true; }
	virtual bool AckCloseCluster(controlserver::AckCloseCluster_Packet &packet) { return true; }
	virtual bool RealtimeRobotState(controlserver::RealtimeRobotState_Packet &packet) { return true; }
};


}
