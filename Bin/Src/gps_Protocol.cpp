#include "pch.h"
#include "gps_Protocol.h"
using namespace gps;

//------------------------------------------------------------------------
// Protocol: GPSInfo
//------------------------------------------------------------------------
void gps::c2s_Protocol::GPSInfo(netid targetId, const double &lon, const double &lat, const float &altitude, const float &speed)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1288261456 );
	packet << lon;
	AddDelimeter(packet);
	packet << lat;
	AddDelimeter(packet);
	packet << altitude;
	AddDelimeter(packet);
	packet << speed;
	AddDelimeter(packet);
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: AddLandMark
//------------------------------------------------------------------------
void gps::c2s_Protocol::AddLandMark(netid targetId, const double &lon, const double &lat)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 4019554964 );
	packet << lon;
	AddDelimeter(packet);
	packet << lat;
	AddDelimeter(packet);
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}



