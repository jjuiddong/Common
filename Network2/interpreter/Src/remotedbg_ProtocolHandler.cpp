#include "stdafx.h"
#include "remotedbg_ProtocolHandler.h"

using namespace remotedbg;


cPacketHeader remotedbg::s2c_Dispatcher::s_packetHeader;
remotedbg::s2c_Dispatcher::s2c_Dispatcher()
	: cProtocolDispatcher(remotedbg::s2c_Dispatcher_ID, ePacketFormat::BINARY)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({s2c_Dispatcher_ID, this});
	cProtocolDispatcher::GetPacketHeaderMap()->insert({s2c_Dispatcher_ID, &s_packetHeader});
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool remotedbg::s2c_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const uint packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 595084065: // AckOneStep
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckOneStep_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckOneStep(data));
		}
		break;

	case 2973361577: // AckDebugRun
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckDebugRun_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckDebugRun(data));
		}
		break;

	case 1020437751: // AckBreak
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckBreak_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckBreak(data));
		}
		break;

	case 1790914558: // AckTerminate
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			AckTerminate_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckTerminate(data));
		}
		break;

	case 344970534: // UpdateInformation
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			UpdateInformation_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.fileName;
			packet >> data.vmName;
			packet >> data.instIndex;
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, UpdateInformation(data));
		}
		break;

	case 1467227726: // UpdateState
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			UpdateState_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.vmName;
			packet >> data.instIndex;
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, UpdateState(data));
		}
		break;

	case 3048159225: // UpdateSymbolTable
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			UpdateSymbolTable_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			packet >> data.start;
			packet >> data.count;
			packet >> data.vmName;
			packet >> data.symbols;
			packet >> data.symbolVals;
			SEND_HANDLER(s2c_ProtocolHandler, prtHandler, UpdateSymbolTable(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}




cPacketHeader remotedbg::c2s_Dispatcher::s_packetHeader;
remotedbg::c2s_Dispatcher::c2s_Dispatcher()
	: cProtocolDispatcher(remotedbg::c2s_Dispatcher_ID, ePacketFormat::BINARY)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({c2s_Dispatcher_ID, this});
	cProtocolDispatcher::GetPacketHeaderMap()->insert({c2s_Dispatcher_ID, &s_packetHeader});
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool remotedbg::c2s_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const uint packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 2870899790: // ReqOneStep
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqOneStep_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqOneStep(data));
		}
		break;

	case 2613943626: // ReqDebugRun
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqDebugRun_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqDebugRun(data));
		}
		break;

	case 2831828176: // ReqBreak
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqBreak_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqBreak(data));
		}
		break;

	case 1343450833: // ReqTerminate
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			ReqTerminate_Packet data;
			data.pdispatcher = this;
			data.senderId = packet.GetSenderId();
			SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqTerminate(data));
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



