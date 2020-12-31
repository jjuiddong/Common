#include "stdafx.h"
#include "gps_Protocol.h"
using namespace gps;

//------------------------------------------------------------------------
// Protocol: GPSInfo
//------------------------------------------------------------------------
void gps::c2s_Protocol::GPSInfo(netid targetId, bool isBinary, const double &lon, const double &lat, const float &altitude, const float &speed, const string &descript)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2000 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, lon);
		marshalling::operator<<(packet, lat);
		marshalling::operator<<(packet, altitude);
		marshalling::operator<<(packet, speed);
		marshalling::operator<<(packet, descript);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "lon", lon);
			put(props, "lat", lat);
			put(props, "altitude", altitude);
			put(props, "speed", speed);
			put(props, "descript", descript);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}



