
#include "stdafx.h"
#include "protocol_udpsvrmap.h"


using namespace udpsvrmap;

cPacketHeader udpsvrmap::Dispatcher::s_packetHeader;


Dispatcher::Dispatcher()
	: cProtocolDispatcher(udpsvrmap::dispatcher_ID, ePacketFormat::BINARY)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({ dispatcher_ID, this });
	cProtocolDispatcher::GetPacketHeaderMap()->insert({ dispatcher_ID, &s_packetHeader });
}


bool udpsvrmap::Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case PACKETID_UDPSVRMAP_CLOSE:
	{
		ProtocolHandlers prtHandler;
		if (!HandlerMatching<ProtocolHandler>(handlers, prtHandler))
			return false;
		SetCurrentDispatchPacket(&packet);
		Close_Packet data;
		data.pdispatcher = this;
		data.senderId = packet.GetSenderId();
		packet >> data.serverName;
		SEND_HANDLER(ProtocolHandler, prtHandler, Close(data));
	}
	break;

	case PACKETID_UDPSVRMAP_ERROR:
	{
		ProtocolHandlers prtHandler;
		if (!HandlerMatching<ProtocolHandler>(handlers, prtHandler))
			return false;
		SetCurrentDispatchPacket(&packet);
		Error_Packet data;
		data.pdispatcher = this;
		data.senderId = packet.GetSenderId();
		packet >> data.serverName;
		packet >> data.errCode;
		SEND_HANDLER(ProtocolHandler, prtHandler, Error(data));
	}
	break;
	default:
		assert(0);
		break;
	}
	return true;
}


// return reserved Close packet
cPacket udpsvrmap::SendClose(network2::cNetworkNode *node)
{
	cPacket packet(GetPacketHeader(ePacketFormat::BINARY));
	packet.SetSenderId(node->m_id);
	packet.SetProtocolId(dispatcher_ID);
	packet.SetPacketId(PACKETID_UDPSVRMAP_CLOSE);
	packet << node->m_name;
	packet.EndPack();
	return packet;
}


// return reserved Error packet
cPacket udpsvrmap::SendError(network2::cNetworkNode *node, int errCode)
{
	cPacket packet(GetPacketHeader(ePacketFormat::BINARY));
	packet.SetSenderId(node->m_id);
	packet.SetProtocolId(dispatcher_ID);
	packet.SetPacketId(PACKETID_UDPSVRMAP_ERROR);
	packet << node->m_name;
	packet << errCode;
	packet.EndPack();
	return packet;
}
