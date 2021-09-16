
#include "stdafx.h"
#include "all_protocol_displayer.h"

using namespace network2;


void AllProtocolDisplayer::RecvPacket(const cPacket &packet)
{
	const int protocolID = packet.GetProtocolId();
	const int packetID = packet.GetPacketId();
	if (m_ignorePackets.end() != m_ignorePackets.find({ protocolID, packetID }))
		return;
	if (m_ignorePackets.end() != m_ignorePackets.find({ protocolID, 0 }))
		return;

	const bool isShow = (
		m_showPackets.empty()
		|| (m_showPackets.end() != m_showPackets.find({ protocolID, 0 }))
		|| (m_showPackets.end() != m_showPackets.find({ protocolID, packetID }))
		);
	
	if (isShow)
		DisplayPacket("Recv = ", packet, m_logLevel);
}
