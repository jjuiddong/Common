//------------------------------------------------------------------------
// Name:    test_ProtocolHandler.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "test_ProtocolData.h"

namespace test {

using namespace network2;
using namespace marshalling;
static const int s2c_Dispatcher_ID = 100000;

// Protocol Dispatcher
class s2c_Dispatcher: public network2::cProtocolDispatcher
{
public:
	s2c_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
	static cPacketHeader s_packetHeader;
};
static s2c_Dispatcher g_test_s2c_Dispatcher;


// ProtocolHandler
class s2c_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class s2c_Dispatcher;
	s2c_ProtocolHandler() { m_format = ePacketFormat::BINARY; }
	virtual bool AckResult(test::AckResult_Packet &packet) { return true; }
};


static const int c2s_Dispatcher_ID = 100001;

// Protocol Dispatcher
class c2s_Dispatcher: public network2::cProtocolDispatcher
{
public:
	c2s_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
	static cPacketHeader s_packetHeader;
};
static c2s_Dispatcher g_test_c2s_Dispatcher;


// ProtocolHandler
class c2s_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class c2s_Dispatcher;
	c2s_ProtocolHandler() { m_format = ePacketFormat::BINARY; }
	virtual bool PacketName1(test::PacketName1_Packet &packet) { return true; }
};


}
