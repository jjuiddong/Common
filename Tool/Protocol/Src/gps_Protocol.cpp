#include "stdafx.h"
#include "gps_Protocol.h"
using namespace gps;

//------------------------------------------------------------------------
// Protocol: GPSInfo
//------------------------------------------------------------------------
void gps::c2s_Protocol::GPSInfo(netid targetId, const double &lon, const double &lat, const float &altitude, const float &speed, const string &descript)
{
	cPacket packet(m_node->GetPacketHeader());
	using boost::property_tree::ptree;
	ptree props;
	try {
		packet.SetProtocolId( GetId() );
		packet.SetPacketId( 2000 );
		put(props, "lon", lon);
		put(props, "lat", lat);
		put(props, "altitude", altitude);
		put(props, "speed", speed);
		put(props, "descript", descript);
		stringstream ss;
		boost::property_tree::write_json(ss, props);
		packet << ss.str();
		packet.EndPack();
	} catch(...) {
	}
	GetNode()->Send(targetId, packet);
}



