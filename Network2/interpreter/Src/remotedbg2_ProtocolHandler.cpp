#include "stdafx.h"
#include "remotedbg2_ProtocolHandler.h"

using namespace remotedbg2;


cPacketHeaderJson remotedbg2::r2h_Dispatcher::s_packetHeader;
remotedbg2::r2h_Dispatcher::r2h_Dispatcher()
	: cProtocolDispatcher(remotedbg2::r2h_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({r2h_Dispatcher_ID, this});
	cProtocolDispatcher::GetPacketHeaderMap()->insert({r2h_Dispatcher_ID, &s_packetHeader});
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool remotedbg2::r2h_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const uint packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 201763535: // UploadIntermediateCode
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				UploadIntermediateCode_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.code);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, UploadIntermediateCode(data));
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

					UploadIntermediateCode_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "code", data.code);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, UploadIntermediateCode(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2564628098: // ReqIntermediateCode
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqIntermediateCode_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqIntermediateCode(data));
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

					ReqIntermediateCode_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqIntermediateCode(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 4041661876: // ReqRun
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqRun_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.runType);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqRun(data));
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

					ReqRun_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "runType", data.runType);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqRun(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 250175983: // ReqOneStep
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqOneStep_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqOneStep(data));
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

					ReqOneStep_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqOneStep(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 433520154: // ReqResumeRun
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqResumeRun_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqResumeRun(data));
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

					ReqResumeRun_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqResumeRun(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1318526848: // ReqBreak
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqBreak_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqBreak(data));
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

					ReqBreak_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqBreak(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3344680000: // ReqBreakPoint
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqBreakPoint_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.enable);
				marshalling::operator>>(packet, data.id);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqBreakPoint(data));
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

					ReqBreakPoint_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "enable", data.enable);
					get(props, "id", data.id);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqBreakPoint(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 4009815483: // ReqStop
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqStop_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqStop(data));
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

					ReqStop_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqStop(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1145831178: // ReqInput
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqInput_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.eventName);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqInput(data));
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

					ReqInput_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "eventName", data.eventName);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqInput(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1246662946: // ReqEvent
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqEvent_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.eventName);
				marshalling::operator>>(packet, data.values);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqEvent(data));
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

					ReqEvent_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "eventName", data.eventName);
					get(props, "values", data.values);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqEvent(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2764328627: // ReqStepDebugType
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqStepDebugType_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.stepDbgType);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqStepDebugType(data));
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

					ReqStepDebugType_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "stepDbgType", data.stepDbgType);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqStepDebugType(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 207775889: // ReqDebugInfo
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqDebugInfo_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.vmIds);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqDebugInfo(data));
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

					ReqDebugInfo_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "vmIds", data.vmIds);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqDebugInfo(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1419406571: // ReqVariableInfo
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqVariableInfo_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.varName);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqVariableInfo(data));
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

					ReqVariableInfo_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "varName", data.varName);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqVariableInfo(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1095285398: // ReqChangeVariable
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqChangeVariable_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.varName);
				marshalling::operator>>(packet, data.value);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqChangeVariable(data));
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

					ReqChangeVariable_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "varName", data.varName);
					get(props, "value", data.value);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqChangeVariable(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1696701698: // ReqVMTree
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqVMTree_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqVMTree(data));
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

					ReqVMTree_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqVMTree(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1935417126: // ReqSyncSymbolTable
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqSyncSymbolTable_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqSyncSymbolTable(data));
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

					ReqSyncSymbolTable_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqSyncSymbolTable(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2149460281: // ReqHeartBeat
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ReqHeartBeat_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqHeartBeat(data));
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

					ReqHeartBeat_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqHeartBeat(data));
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




cPacketHeaderJson remotedbg2::h2r_Dispatcher::s_packetHeader;
remotedbg2::h2r_Dispatcher::h2r_Dispatcher()
	: cProtocolDispatcher(remotedbg2::h2r_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({h2r_Dispatcher_ID, this});
	cProtocolDispatcher::GetPacketHeaderMap()->insert({h2r_Dispatcher_ID, &s_packetHeader});
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool remotedbg2::h2r_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const uint packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 3047429: // Welcome
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
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
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, Welcome(data));
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
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, Welcome(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2462931757: // AckUploadIntermediateCode
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckUploadIntermediateCode_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckUploadIntermediateCode(data));
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

					AckUploadIntermediateCode_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckUploadIntermediateCode(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3529554811: // AckIntermediateCode
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckIntermediateCode_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.result);
				marshalling::operator>>(packet, data.count);
				marshalling::operator>>(packet, data.index);
				marshalling::operator>>(packet, data.totalBufferSize);
				marshalling::operator>>(packet, data.data);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckIntermediateCode(data));
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

					AckIntermediateCode_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "result", data.result);
					get(props, "count", data.count);
					get(props, "index", data.index);
					get(props, "totalBufferSize", data.totalBufferSize);
					get(props, "data", data.data);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckIntermediateCode(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 641506255: // SpawnTotalInterpreterInfo
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SpawnTotalInterpreterInfo_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.totalCount);
				marshalling::operator>>(packet, data.index);
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmIds);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SpawnTotalInterpreterInfo(data));
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

					SpawnTotalInterpreterInfo_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "totalCount", data.totalCount);
					get(props, "index", data.index);
					get(props, "itprId", data.itprId);
					get(props, "vmIds", data.vmIds);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SpawnTotalInterpreterInfo(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3321369215: // SpawnInterpreterInfo
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SpawnInterpreterInfo_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.parentVmId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.nodeFileName);
				marshalling::operator>>(packet, data.nodeName);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SpawnInterpreterInfo(data));
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

					SpawnInterpreterInfo_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "parentVmId", data.parentVmId);
					get(props, "vmId", data.vmId);
					get(props, "nodeFileName", data.nodeFileName);
					get(props, "nodeName", data.nodeName);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SpawnInterpreterInfo(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1233636689: // RemoveInterpreter
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				RemoveInterpreter_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.vmId);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, RemoveInterpreter(data));
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

					RemoveInterpreter_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "vmId", data.vmId);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, RemoveInterpreter(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3610915871: // AckRun
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckRun_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckRun(data));
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

					AckRun_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckRun(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3508334431: // AckOneStep
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckOneStep_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckOneStep(data));
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

					AckOneStep_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckOneStep(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 599066917: // AckResumeRun
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckResumeRun_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckResumeRun(data));
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

					AckResumeRun_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckResumeRun(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3655568643: // AckBreak
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckBreak_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckBreak(data));
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

					AckBreak_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckBreak(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3327558841: // AckBreakPoint
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckBreakPoint_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.enable);
				marshalling::operator>>(packet, data.id);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckBreakPoint(data));
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

					AckBreakPoint_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "enable", data.enable);
					get(props, "id", data.id);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckBreakPoint(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3614765391: // AckStop
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckStop_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckStop(data));
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

					AckStop_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckStop(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3422905177: // AckInput
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckInput_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckInput(data));
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

					AckInput_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckInput(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 4273588753: // AckEvent
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckEvent_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.eventName);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckEvent(data));
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

					AckEvent_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "eventName", data.eventName);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckEvent(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 831321155: // AckStepDebugType
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckStepDebugType_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.stepDbgType);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckStepDebugType(data));
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

					AckStepDebugType_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "stepDbgType", data.stepDbgType);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckStepDebugType(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1917526265: // AckDebugInfo
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckDebugInfo_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.vmIds);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckDebugInfo(data));
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

					AckDebugInfo_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "vmIds", data.vmIds);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckDebugInfo(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 910405673: // AckChangeVariable
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckChangeVariable_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.varName);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckChangeVariable(data));
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

					AckChangeVariable_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "varName", data.varName);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckChangeVariable(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3389117266: // AckVMTree
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckVMTree_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.id);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckVMTree(data));
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

					AckVMTree_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "id", data.id);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckVMTree(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3768290937: // AckVMTreeStream
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckVMTreeStream_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.id);
				marshalling::operator>>(packet, data.count);
				marshalling::operator>>(packet, data.index);
				marshalling::operator>>(packet, data.totalBufferSize);
				marshalling::operator>>(packet, data.data);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckVMTreeStream(data));
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

					AckVMTreeStream_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "id", data.id);
					get(props, "count", data.count);
					get(props, "index", data.index);
					get(props, "totalBufferSize", data.totalBufferSize);
					get(props, "data", data.data);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckVMTreeStream(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2019731778: // AckSyncSymbolTable
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckSyncSymbolTable_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckSyncSymbolTable(data));
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

					AckSyncSymbolTable_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckSyncSymbolTable(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1151204869: // AckSymbolTableStream
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckSymbolTableStream_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.count);
				marshalling::operator>>(packet, data.index);
				marshalling::operator>>(packet, data.totalBufferSize);
				marshalling::operator>>(packet, data.data);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckSymbolTableStream(data));
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

					AckSymbolTableStream_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "count", data.count);
					get(props, "index", data.index);
					get(props, "totalBufferSize", data.totalBufferSize);
					get(props, "data", data.data);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckSymbolTableStream(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 601265629: // SyncVMInstruction
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMInstruction_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.indices);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMInstruction(data));
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

					SyncVMInstruction_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "indices", data.indices);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMInstruction(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 398018375: // SyncVMRegister
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMRegister_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.infoType);
				marshalling::operator>>(packet, data.reg);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMRegister(data));
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

					SyncVMRegister_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "infoType", data.infoType);
					get(props, "reg", data.reg);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMRegister(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 20168650: // SyncVMSymbolTable
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMSymbolTable_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.start);
				marshalling::operator>>(packet, data.count);
				marshalling::operator>>(packet, data.symbol);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMSymbolTable(data));
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

					SyncVMSymbolTable_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "start", data.start);
					get(props, "count", data.count);
					get(props, "symbol", data.symbol);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMSymbolTable(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2811960017: // SyncVMOutput
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMOutput_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.output);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMOutput(data));
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

					SyncVMOutput_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "output", data.output);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMOutput(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2243507416: // SyncVMWidgets
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMWidgets_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.widgetName);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMWidgets(data));
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

					SyncVMWidgets_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "widgetName", data.widgetName);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMWidgets(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1959519461: // SyncVMArray
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMArray_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.varName);
				marshalling::operator>>(packet, data.startIdx);
				marshalling::operator>>(packet, data.totalSize);
				marshalling::operator>>(packet, data.array);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMArray(data));
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

					SyncVMArray_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "varName", data.varName);
					get(props, "startIdx", data.startIdx);
					get(props, "totalSize", data.totalSize);
					get(props, "array", data.array);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMArray(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3675896516: // SyncVMArrayBool
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMArrayBool_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.varName);
				marshalling::operator>>(packet, data.startIdx);
				marshalling::operator>>(packet, data.totalSize);
				marshalling::operator>>(packet, data.array);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMArrayBool(data));
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

					SyncVMArrayBool_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "varName", data.varName);
					get(props, "startIdx", data.startIdx);
					get(props, "totalSize", data.totalSize);
					get(props, "array", data.array);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMArrayBool(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1234136920: // SyncVMArrayNumber
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMArrayNumber_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.varName);
				marshalling::operator>>(packet, data.startIdx);
				marshalling::operator>>(packet, data.totalSize);
				marshalling::operator>>(packet, data.array);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMArrayNumber(data));
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

					SyncVMArrayNumber_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "varName", data.varName);
					get(props, "startIdx", data.startIdx);
					get(props, "totalSize", data.totalSize);
					get(props, "array", data.array);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMArrayNumber(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1603321848: // SyncVMArrayString
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMArrayString_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.varName);
				marshalling::operator>>(packet, data.startIdx);
				marshalling::operator>>(packet, data.totalSize);
				marshalling::operator>>(packet, data.array);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMArrayString(data));
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

					SyncVMArrayString_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "varName", data.varName);
					get(props, "startIdx", data.startIdx);
					get(props, "totalSize", data.totalSize);
					get(props, "array", data.array);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMArrayString(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 4149275049: // SyncVMTimer
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMTimer_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmId);
				marshalling::operator>>(packet, data.scopeName);
				marshalling::operator>>(packet, data.timerId);
				marshalling::operator>>(packet, data.time);
				marshalling::operator>>(packet, data.actionType);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMTimer(data));
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

					SyncVMTimer_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmId", data.vmId);
					get(props, "scopeName", data.scopeName);
					get(props, "timerId", data.timerId);
					get(props, "time", data.time);
					get(props, "actionType", data.actionType);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMTimer(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 445946481: // ExecuteCustomFunction
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				ExecuteCustomFunction_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.fnName);
				marshalling::operator>>(packet, data.args);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, ExecuteCustomFunction(data));
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

					ExecuteCustomFunction_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "fnName", data.fnName);
					get(props, "args", data.args);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, ExecuteCustomFunction(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2075525090: // AckHeartBeat
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckHeartBeat_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckHeartBeat(data));
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

					AckHeartBeat_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckHeartBeat(data));
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



