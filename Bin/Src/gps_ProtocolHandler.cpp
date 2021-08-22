#include "pch.h"
#include "gps_ProtocolHandler.h"

using namespace gps;


gps::c2s_Dispatcher::c2s_Dispatcher()
	: cProtocolDispatcher(gps::c2s_Dispatcher_ID, ePacketFormat::BINARY)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({c2s_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool gps::c2s_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 1288261456: // GPSInfo
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			GPSInfo_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.lon;
			packet >> data.lat;
			packet >> data.altitude;
			packet >> data.speed;
			SEND_HANDLER(c2s_ProtocolHandler, prtHandler, GPSInfo(data));
		}
		break;

	case 4019554964: // AddLandMark
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			AddLandMark_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.lon;
			packet >> data.lat;
			SEND_HANDLER(c2s_ProtocolHandler, prtHandler, AddLandMark(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



