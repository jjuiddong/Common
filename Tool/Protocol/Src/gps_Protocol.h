//------------------------------------------------------------------------
// Name:    gps_Protocol.h
// Author:  ProtocolGenerator (by jjuiddong)
// Date:    
//------------------------------------------------------------------------
#pragma once

namespace gps {

using namespace network2;
using namespace marshalling_json;
static const int c2s_Protocol_ID = 1000;

class c2s_Protocol : public network2::iProtocol
{
public:
	c2s_Protocol() : iProtocol(c2s_Protocol_ID, ePacketFormat::JSON) {}
	void GPSInfo(netid targetId, bool isBinary, const double &lon, const double &lat, const float &altitude, const float &speed, const string &descript);
};
}
