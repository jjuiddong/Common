//------------------------------------------------------------------------
// Name:    editor_ProtocolHandler.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "editor_ProtocolData.h"

namespace editor {

using namespace network2;
using namespace marshalling_json;
static const int s2c_Dispatcher_ID = 9000;

// Protocol Dispatcher
class s2c_Dispatcher: public network2::cProtocolDispatcher
{
public:
	s2c_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static s2c_Dispatcher g_editor_s2c_Dispatcher;


// ProtocolHandler
class s2c_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class s2c_Dispatcher;
	s2c_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool AckRunSimulation(editor::AckRunSimulation_Packet &packet) { return true; }
	virtual bool AckRunProject(editor::AckRunProject_Packet &packet) { return true; }
	virtual bool AckStopSimulation(editor::AckStopSimulation_Packet &packet) { return true; }
};


static const int c2s_Dispatcher_ID = 9100;

// Protocol Dispatcher
class c2s_Dispatcher: public network2::cProtocolDispatcher
{
public:
	c2s_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static c2s_Dispatcher g_editor_c2s_Dispatcher;


// ProtocolHandler
class c2s_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class c2s_Dispatcher;
	c2s_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool ReqRunSimulation(editor::ReqRunSimulation_Packet &packet) { return true; }
	virtual bool ReqRunProject(editor::ReqRunProject_Packet &packet) { return true; }
	virtual bool ReqStopSimulation(editor::ReqStopSimulation_Packet &packet) { return true; }
};


}
