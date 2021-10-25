#include "pch.h"
#include "test_ProtocolHandler.h"

using namespace test;


cPacketHeader test::s2c_Dispatcher::s_packetHeader;
test::s2c_Dispatcher::s2c_Dispatcher()
	: cProtocolDispatcher(test::s2c_Dispatcher_ID, ePacketFormat::BINARY)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({s2c_Dispatcher_ID, this});
	cProtocolDispatcher::GetPacketHeaderMap()->insert({s2c_Dispatcher_ID, &s_packetHeader});
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool test::s2c_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 3846277350: // AckResult
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckResult_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.packetName;
			packet >> data.result;
			packet >> data.vars;
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckResult(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}




cPacketHeader test::c2s_Dispatcher::s_packetHeader;
test::c2s_Dispatcher::c2s_Dispatcher()
	: cProtocolDispatcher(test::c2s_Dispatcher_ID, ePacketFormat::BINARY)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({c2s_Dispatcher_ID, this});
	cProtocolDispatcher::GetPacketHeaderMap()->insert({c2s_Dispatcher_ID, &s_packetHeader});
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool test::c2s_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 2900479863: // PacketName1
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			PacketName1_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.id;
			packet >> data.data;
			packet >> data.num;
			SEND_HANDLER(c2s_ProtocolHandler, prtHandler, PacketName1(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



