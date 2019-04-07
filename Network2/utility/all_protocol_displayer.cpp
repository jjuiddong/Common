
#include "stdafx.h"
#include "all_protocol_displayer.h"

using namespace network2;


void AllProtocolDisplayer::RecvPacket(const cPacket &packet)
{
	const int protocolID = packet.GetProtocolId();
	const int packetID = packet.GetPacketId();
	if (m_ignorePackets.end() != m_ignorePackets.find({ protocolID, packetID }))
		return;

	DisplayPacket("Recv = ", packet, m_logLevel);
}
