//------------------------------------------------------------------------
// Name:    remotedbg_ProtocolHandler.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "remotedbg_ProtocolData.h"

namespace remotedbg {

using namespace network2;
using namespace marshalling;
static const int s2c_Dispatcher_ID = 5200;

// Protocol Dispatcher
class s2c_Dispatcher: public network2::cProtocolDispatcher
{
public:
	s2c_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static s2c_Dispatcher g_remotedbg_s2c_Dispatcher;


// ProtocolHandler
class s2c_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class s2c_Dispatcher;
	s2c_ProtocolHandler() { m_format = ePacketFormat::BINARY; }
	virtual bool AckOneStep(remotedbg::AckOneStep_Packet &packet) { return true; }
	virtual bool AckDebugRun(remotedbg::AckDebugRun_Packet &packet) { return true; }
	virtual bool AckBreak(remotedbg::AckBreak_Packet &packet) { return true; }
	virtual bool AckTerminate(remotedbg::AckTerminate_Packet &packet) { return true; }
	virtual bool UpdateInformation(remotedbg::UpdateInformation_Packet &packet) { return true; }
	virtual bool UpdateState(remotedbg::UpdateState_Packet &packet) { return true; }
	virtual bool UpdateSymbolTable(remotedbg::UpdateSymbolTable_Packet &packet) { return true; }
};


static const int c2s_Dispatcher_ID = 5201;

// Protocol Dispatcher
class c2s_Dispatcher: public network2::cProtocolDispatcher
{
public:
	c2s_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static c2s_Dispatcher g_remotedbg_c2s_Dispatcher;


// ProtocolHandler
class c2s_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class c2s_Dispatcher;
	c2s_ProtocolHandler() { m_format = ePacketFormat::BINARY; }
	virtual bool ReqOneStep(remotedbg::ReqOneStep_Packet &packet) { return true; }
	virtual bool ReqDebugRun(remotedbg::ReqDebugRun_Packet &packet) { return true; }
	virtual bool ReqBreak(remotedbg::ReqBreak_Packet &packet) { return true; }
	virtual bool ReqTerminate(remotedbg::ReqTerminate_Packet &packet) { return true; }
};


}
