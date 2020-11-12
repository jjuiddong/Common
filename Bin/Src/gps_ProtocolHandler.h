//------------------------------------------------------------------------
// Name:    gps_ProtocolHandler.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

#include "gps_ProtocolData.h"

namespace gps {

using namespace network2;
using namespace marshalling;
static const int c2s_Dispatcher_ID = 1000;

// Protocol Dispatcher
class c2s_Dispatcher: public network2::cProtocolDispatcher
{
public:
	c2s_Dispatcher();
protected:
	virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
};
static c2s_Dispatcher g_gps_c2s_Dispatcher;


// ProtocolHandler
class c2s_ProtocolHandler : virtual public network2::iProtocolHandler
{
public:
	friend class c2s_Dispatcher;
	c2s_ProtocolHandler() { m_format = ePacketFormat::BINARY; }
	virtual bool GPSInfo(gps::GPSInfo_Packet &packet) { return true; }
	virtual bool AddLandMark(gps::AddLandMark_Packet &packet) { return true; }
};


}
