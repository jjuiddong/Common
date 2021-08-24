#include "pch.h"
#include "clustersync_ProtocolHandler.h"

using namespace clustersync;


clustersync::s2c_Dispatcher::s2c_Dispatcher()
	: cProtocolDispatcher(clustersync::s2c_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({s2c_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool clustersync::s2c_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	return true;
}




clustersync::c2s_Dispatcher::c2s_Dispatcher()
	: cProtocolDispatcher(clustersync::c2s_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({c2s_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool clustersync::c2s_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 2663605951: // SpawnRobot
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SpawnRobot_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.robotName);
				marshalling::operator>>(packet, data.modelName);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, SpawnRobot(data));
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

					SpawnRobot_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "robotName", data.robotName);
					get(props, "modelName", data.modelName);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, SpawnRobot(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 27410734: // RemoveRobot
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				RemoveRobot_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.robotName);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, RemoveRobot(data));
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

					RemoveRobot_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "robotName", data.robotName);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, RemoveRobot(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2061430798: // RobotWorkFinish
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				RobotWorkFinish_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.robotName);
				marshalling::operator>>(packet, data.taskId);
				marshalling::operator>>(packet, data.totalWorkCount);
				marshalling::operator>>(packet, data.finishWorkCount);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, RobotWorkFinish(data));
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

					RobotWorkFinish_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "robotName", data.robotName);
					get(props, "taskId", data.taskId);
					get(props, "totalWorkCount", data.totalWorkCount);
					get(props, "finishWorkCount", data.finishWorkCount);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, RobotWorkFinish(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1079519326: // RealtimeRobotState
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<c2s_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				RealtimeRobotState_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.robotName);
				marshalling::operator>>(packet, data.state1);
				marshalling::operator>>(packet, data.state2);
				marshalling::operator>>(packet, data.state3);
				marshalling::operator>>(packet, data.state4);
				marshalling::operator>>(packet, data.pos);
				marshalling::operator>>(packet, data.dir);
				SEND_HANDLER(c2s_ProtocolHandler, prtHandler, RealtimeRobotState(data));
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

					RealtimeRobotState_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "robotName", data.robotName);
					get(props, "state1", data.state1);
					get(props, "state2", data.state2);
					get(props, "state3", data.state3);
					get(props, "state4", data.state4);
					get(props, "pos", data.pos);
					get(props, "dir", data.dir);
					SEND_HANDLER(c2s_ProtocolHandler, prtHandler, RealtimeRobotState(data));
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



