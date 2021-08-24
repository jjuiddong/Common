//------------------------------------------------------------------------
// Name:    visualprogram_ProtocolHandler.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "visualprogram_ProtocolData.h"

namespace visualprogram {

using namespace network2;
using namespace marshalling_json;
static const int s2w_Dispatcher_ID = 2100;

// Protocol Dispatcher
class s2w_Dispatcher: public network2::cProtocolDispatcher
{
public:
	s2w_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static s2w_Dispatcher g_visualprogram_s2w_Dispatcher;


// ProtocolHandler
class s2w_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class s2w_Dispatcher;
	s2w_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool AckWebLogin(visualprogram::AckWebLogin_Packet &packet) { return true; }
	virtual bool AckWebRunVisualProgStream(visualprogram::AckWebRunVisualProgStream_Packet &packet) { return true; }
	virtual bool AckWebStopVisualProg(visualprogram::AckWebStopVisualProg_Packet &packet) { return true; }
	virtual bool ReqWebConnectRIProxyServer(visualprogram::ReqWebConnectRIProxyServer_Packet &packet) { return true; }
};


static const int w2s_Dispatcher_ID = 2101;

// Protocol Dispatcher
class w2s_Dispatcher: public network2::cProtocolDispatcher
{
public:
	w2s_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static w2s_Dispatcher g_visualprogram_w2s_Dispatcher;


// ProtocolHandler
class w2s_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class w2s_Dispatcher;
	w2s_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool ReqWebLogin(visualprogram::ReqWebLogin_Packet &packet) { return true; }
	virtual bool ReqWebRunVisualProgStream(visualprogram::ReqWebRunVisualProgStream_Packet &packet) { return true; }
	virtual bool ReqWebStopVisualProg(visualprogram::ReqWebStopVisualProg_Packet &packet) { return true; }
};


static const int s2c_Dispatcher_ID = 1000;

// Protocol Dispatcher
class s2c_Dispatcher: public network2::cProtocolDispatcher
{
public:
	s2c_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static s2c_Dispatcher g_visualprogram_s2c_Dispatcher;


// ProtocolHandler
class s2c_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class s2c_Dispatcher;
	s2c_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool Welcome(visualprogram::Welcome_Packet &packet) { return true; }
	virtual bool AckLogin(visualprogram::AckLogin_Packet &packet) { return true; }
	virtual bool ReqRunVisualProg(visualprogram::ReqRunVisualProg_Packet &packet) { return true; }
	virtual bool ReqRunVisualProgStream(visualprogram::ReqRunVisualProgStream_Packet &packet) { return true; }
	virtual bool ReqRunVisualProgFiles(visualprogram::ReqRunVisualProgFiles_Packet &packet) { return true; }
	virtual bool ReqStopVisualProg(visualprogram::ReqStopVisualProg_Packet &packet) { return true; }
	virtual bool ReqConnectRIProxyServer(visualprogram::ReqConnectRIProxyServer_Packet &packet) { return true; }
};


static const int c2s_Dispatcher_ID = 2000;

// Protocol Dispatcher
class c2s_Dispatcher: public network2::cProtocolDispatcher
{
public:
	c2s_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static c2s_Dispatcher g_visualprogram_c2s_Dispatcher;


// ProtocolHandler
class c2s_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class c2s_Dispatcher;
	c2s_ProtocolHandler() { m_format = ePacketFormat::JSON; }
	virtual bool ReqLogin(visualprogram::ReqLogin_Packet &packet) { return true; }
	virtual bool AckRunVisualProg(visualprogram::AckRunVisualProg_Packet &packet) { return true; }
	virtual bool AckRunVisualProgStream(visualprogram::AckRunVisualProgStream_Packet &packet) { return true; }
	virtual bool AckRunVisualProgFiles(visualprogram::AckRunVisualProgFiles_Packet &packet) { return true; }
	virtual bool AckStopVisualProg(visualprogram::AckStopVisualProg_Packet &packet) { return true; }
	virtual bool AckConnectRIProxyServer(visualprogram::AckConnectRIProxyServer_Packet &packet) { return true; }
};


}
