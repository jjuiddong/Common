#include "basic_ProtocolHandler.h"

using namespace basic;


basic::s2c_Dispatcher::s2c_Dispatcher()
	: cProtocolDispatcher(basic::s2c_Dispatcher_ID)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({s2c_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool basic::s2c_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 3587614684:
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			func1_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, func1(data));
		}
		break;

	case 2592878926:
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			func2_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.str;
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, func2(data));
		}
		break;

	case 3387225200:
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			func3_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.value;
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, func3(data));
		}
		break;

	case 4124054319:
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			func4_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, func4(data));
		}
		break;

	case 622789402:
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			func5_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.name;
			packet >> data.ar;
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, func5(data));
		}
		break;

	case 1506470321:
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			func6_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.ar;
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, func6(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}




basic::c2s_Dispatcher::c2s_Dispatcher()
	: cProtocolDispatcher(basic::c2s_Dispatcher_ID)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({c2s_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool basic::c2s_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 622789402:
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			func5_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.str;
			SEND_HANDLER(c2s_ProtocolHandler, prtHandler, func5(data));
		}
		break;

	case 1506470321:
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			func6_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.value;
			SEND_HANDLER(c2s_ProtocolHandler, prtHandler, func6(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



