#include "pch.h"
#include "editor_ProtocolHandler.h"

using namespace editor;


editor::s2c_Dispatcher::s2c_Dispatcher()
	: cProtocolDispatcher(editor::s2c_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({s2c_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool editor::s2c_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 207562622: // AckRunSimulation
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckRunSimulation_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.projectName);
				marshalling::operator>>(packet, data.mapFileName);
				marshalling::operator>>(packet, data.pathFileName);
				marshalling::operator>>(packet, data.nodeFileNames);
				marshalling::operator>>(packet, data.taskName);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckRunSimulation(data));
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

					AckRunSimulation_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "projectName", data.projectName);
					get(props, "mapFileName", data.mapFileName);
					get(props, "pathFileName", data.pathFileName);
					get(props, "nodeFileNames", data.nodeFileNames);
					get(props, "taskName", data.taskName);
					get(props, "result", data.result);
					SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckRunSimulation(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1279498047: // AckRunProject
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckRunProject_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.projectName);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckRunProject(data));
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

					AckRunProject_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "projectName", data.projectName);
					get(props, "result", data.result);
					SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckRunProject(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 4276051529: // AckStopSimulation
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<s2c_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckStopSimulation_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckStopSimulation(data));
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

					AckStopSimulation_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "result", data.result);
					SEND_HANDLER(s2c_ProtocolHandler, prtHandler, AckStopSimulation(data));
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




editor::c2s_Dispatcher::c2s_Dispatcher()
	: cProtocolDispatcher(editor::c2s_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({c2s_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool editor::c2s_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 2415569453: // ReqRunSimulation
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqRunSimulation_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.projectName);
				marshalling::operator>>(packet, data.mapFileName);
				marshalling::operator>>(packet, data.pathFileName);
				marshalling::operator>>(packet, data.nodeFileNames);
				marshalling::operator>>(packet, data.taskName);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqRunSimulation(data));
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

					ReqRunSimulation_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "projectName", data.projectName);
					get(props, "mapFileName", data.mapFileName);
					get(props, "pathFileName", data.pathFileName);
					get(props, "nodeFileNames", data.nodeFileNames);
					get(props, "taskName", data.taskName);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqRunSimulation(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2272961170: // ReqRunProject
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqRunProject_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.projectName);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqRunProject(data));
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

					ReqRunProject_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "projectName", data.projectName);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqRunProject(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3081823622: // ReqStopSimulation
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqStopSimulation_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqStopSimulation(data));
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

					ReqStopSimulation_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, ReqStopSimulation(data));
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



