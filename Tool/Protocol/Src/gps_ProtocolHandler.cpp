#include "stdafx.h"
#include "gps_ProtocolHandler.h"

using namespace gps;


gps::c2s_Dispatcher::c2s_Dispatcher()
	: cProtocolDispatcher(gps::c2s_Dispatcher_ID, ePacketFormat::JSON)
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
	case 2000:
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			// json format packet parsing using property_tree
			using boost::property_tree::ptree;
			ptree root;

			try {
				string str;
				packet >> str;
				stringstream ss(str);
				
				boost::property_tree::read_json(ss, root);
				ptree &props = root.get_child("");

				GPSInfo_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				get(props, "lon", data.lon);
				get(props, "lat", data.lat);
				get(props, "altitude", data.altitude);
				get(props, "speed", data.speed);
				get(props, "descript", data.descript);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, GPSInfo(data));
			} catch (...) {
				dbg::Logp("json packet parsing error\n");
			}
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



