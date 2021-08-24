#include "pch.h"
#include "visualprogram_ProtocolHandler.h"

using namespace visualprogram;


visualprogram::s2w_Dispatcher::s2w_Dispatcher()
	: cProtocolDispatcher(visualprogram::s2w_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({s2w_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool visualprogram::s2w_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 1933022727: // AckWebLogin
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2w_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckWebLogin_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.id);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(s2w_ProtocolHandler, prtHandler, AckWebLogin(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckWebLogin_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "id", data.id);
					get(props, "result", data.result);
					SEND_HANDLER(s2w_ProtocolHandler, prtHandler, AckWebLogin(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 4236112087: // AckWebRunVisualProgStream
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2w_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckWebRunVisualProgStream_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.nodeFileName);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(s2w_ProtocolHandler, prtHandler, AckWebRunVisualProgStream(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckWebRunVisualProgStream_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "nodeFileName", data.nodeFileName);
					get(props, "result", data.result);
					SEND_HANDLER(s2w_ProtocolHandler, prtHandler, AckWebRunVisualProgStream(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 4248820795: // AckWebStopVisualProg
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2w_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckWebStopVisualProg_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(s2w_ProtocolHandler, prtHandler, AckWebStopVisualProg(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckWebStopVisualProg_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "result", data.result);
					SEND_HANDLER(s2w_ProtocolHandler, prtHandler, AckWebStopVisualProg(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2874116538: // ReqWebConnectRIProxyServer
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2w_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqWebConnectRIProxyServer_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.url);
				marshalling::operator>>(packet, data.name);
				SEND_HANDLER(s2w_ProtocolHandler, prtHandler, ReqWebConnectRIProxyServer(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					ReqWebConnectRIProxyServer_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "url", data.url);
					get(props, "name", data.name);
					SEND_HANDLER(s2w_ProtocolHandler, prtHandler, ReqWebConnectRIProxyServer(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}




visualprogram::w2s_Dispatcher::w2s_Dispatcher()
	: cProtocolDispatcher(visualprogram::w2s_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({w2s_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool visualprogram::w2s_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 4200687688: // ReqWebLogin
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<w2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqWebLogin_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.id);
				SEND_HANDLER(w2s_ProtocolHandler, prtHandler, ReqWebLogin(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					ReqWebLogin_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "id", data.id);
					SEND_HANDLER(w2s_ProtocolHandler, prtHandler, ReqWebLogin(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 994683893: // ReqWebRunVisualProgStream
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<w2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqWebRunVisualProgStream_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.nodeFileName);
				SEND_HANDLER(w2s_ProtocolHandler, prtHandler, ReqWebRunVisualProgStream(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					ReqWebRunVisualProgStream_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "nodeFileName", data.nodeFileName);
					SEND_HANDLER(w2s_ProtocolHandler, prtHandler, ReqWebRunVisualProgStream(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1431744851: // ReqWebStopVisualProg
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<w2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqWebStopVisualProg_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				SEND_HANDLER(w2s_ProtocolHandler, prtHandler, ReqWebStopVisualProg(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					ReqWebStopVisualProg_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					SEND_HANDLER(w2s_ProtocolHandler, prtHandler, ReqWebStopVisualProg(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}




visualprogram::s2c_Dispatcher::s2c_Dispatcher()
	: cProtocolDispatcher(visualprogram::s2c_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({s2c_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool visualprogram::s2c_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 1281093745: // Welcome
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				Welcome_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.msg);
				SEND_HANDLER(s2c_ProtocolHandler, prtHandler, Welcome(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					Welcome_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "msg", data.msg);
					SEND_HANDLER(s2c_ProtocolHandler, prtHandler, Welcome(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 851424104: // AckLogin
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckLogin_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.id);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckLogin(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckLogin_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "id", data.id);
					get(props, "result", data.result);
					SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckLogin(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2250021743: // ReqRunVisualProg
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqRunVisualProg_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.nodeFile);
				SEND_HANDLER(s2c_ProtocolHandler, prtHandler, ReqRunVisualProg(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					ReqRunVisualProg_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "nodeFile", data.nodeFile);
					SEND_HANDLER(s2c_ProtocolHandler, prtHandler, ReqRunVisualProg(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3686541167: // ReqRunVisualProgStream
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqRunVisualProgStream_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.count);
				marshalling::operator>>(packet, data.index);
				marshalling::operator>>(packet, data.data);
				SEND_HANDLER(s2c_ProtocolHandler, prtHandler, ReqRunVisualProgStream(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					ReqRunVisualProgStream_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "count", data.count);
					get(props, "index", data.index);
					get(props, "data", data.data);
					SEND_HANDLER(s2c_ProtocolHandler, prtHandler, ReqRunVisualProgStream(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3721131371: // ReqRunVisualProgFiles
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqRunVisualProgFiles_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.nodeFileNames);
				SEND_HANDLER(s2c_ProtocolHandler, prtHandler, ReqRunVisualProgFiles(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					ReqRunVisualProgFiles_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "nodeFileNames", data.nodeFileNames);
					SEND_HANDLER(s2c_ProtocolHandler, prtHandler, ReqRunVisualProgFiles(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 4258374867: // ReqStopVisualProg
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqStopVisualProg_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				SEND_HANDLER(s2c_ProtocolHandler, prtHandler, ReqStopVisualProg(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					ReqStopVisualProg_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					SEND_HANDLER(s2c_ProtocolHandler, prtHandler, ReqStopVisualProg(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3492118321: // ReqConnectRIProxyServer
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqConnectRIProxyServer_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.url);
				marshalling::operator>>(packet, data.name);
				SEND_HANDLER(s2c_ProtocolHandler, prtHandler, ReqConnectRIProxyServer(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					ReqConnectRIProxyServer_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "url", data.url);
					get(props, "name", data.name);
					SEND_HANDLER(s2c_ProtocolHandler, prtHandler, ReqConnectRIProxyServer(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}




visualprogram::c2s_Dispatcher::c2s_Dispatcher()
	: cProtocolDispatcher(visualprogram::c2s_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({c2s_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool visualprogram::c2s_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 1956887904: // ReqLogin
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqLogin_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.id);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqLogin(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					ReqLogin_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "id", data.id);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqLogin(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3863877132: // AckRunVisualProg
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckRunVisualProg_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, AckRunVisualProg(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckRunVisualProg_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "result", data.result);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, AckRunVisualProg(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3454830338: // AckRunVisualProgStream
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckRunVisualProgStream_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, AckRunVisualProgStream(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckRunVisualProgStream_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "result", data.result);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, AckRunVisualProgStream(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2713087572: // AckRunVisualProgFiles
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckRunVisualProgFiles_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, AckRunVisualProgFiles(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckRunVisualProgFiles_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "result", data.result);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, AckRunVisualProgFiles(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1895439953: // AckStopVisualProg
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckStopVisualProg_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, AckStopVisualProg(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckStopVisualProg_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "result", data.result);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, AckStopVisualProg(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3286171390: // AckConnectRIProxyServer
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckConnectRIProxyServer_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.url);
				marshalling::operator>>(packet, data.name);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, AckConnectRIProxyServer(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckConnectRIProxyServer_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "url", data.url);
					get(props, "name", data.name);
					get(props, "result", data.result);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, AckConnectRIProxyServer(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}



