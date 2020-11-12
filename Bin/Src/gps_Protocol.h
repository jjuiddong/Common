//------------------------------------------------------------------------
// Name:    gps_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace gps {

using namespace network2;
using namespace marshalling;
static const int c2s_Protocol_ID = 1000;

class c2s_Protocol : public network2::iProtocol
{
public:
	c2s_Protocol() : iProtocol(c2s_Protocol_ID, ePacketFormat::BINARY) {}
	void GPSInfo(netid targetId, const double &lon, const double &lat, const float &altitude, const float &speed);
	void AddLandMark(netid targetId, const double &lon, const double &lat);
};
}
