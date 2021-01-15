#include "stdafx.h"
#include "remotedbg2_ProtocolHandler.h"

using namespace remotedbg2;


remotedbg2::r2h_Dispatcher::r2h_Dispatcher()
	: cProtocolDispatcher(remotedbg2::r2h_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({r2h_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool remotedbg2::r2h_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 1281093745: // Welcome
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
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
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, Welcome(data));
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
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, Welcome(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 3405729511: // UploadVProgFile
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<r2h_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				UploadVProgFile_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.code);
				SEND_HANDLER(r2h_ProtocolHandler, prtHandler, UploadVProgFile(data));
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

					UploadVProgFile_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "code", data.code);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, UploadVProgFile(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 1644585100: // ReqIntermediateCode
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
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqIntermediateCode(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 923151823: // ReqRun
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
					get(props, "runType", data.runType);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqRun(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 2884814738: // ReqOneStep
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
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqOneStep(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 742173167: // ReqResumeRun
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
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqResumeRun(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 784411795: // ReqBreak
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
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqBreak(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 1453251868: // ReqStop
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
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqStop(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 3140751413: // ReqInput
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
				marshalling::operator>>(packet, data.vmIdx);
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
					get(props, "vmIdx", data.vmIdx);
					get(props, "eventName", data.eventName);
					SEND_HANDLER(r2h_ProtocolHandler, prtHandler, ReqInput(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 2532286881: // ReqHeartBeat
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
					dbg::Logp("json packet parsing error\n");
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




remotedbg2::h2r_Dispatcher::h2r_Dispatcher()
	: cProtocolDispatcher(remotedbg2::h2r_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({h2r_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// 패킷의 프로토콜에 따라 해당하는 핸들러를 호출한다.
//------------------------------------------------------------------------
bool remotedbg2::h2r_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 2201380897: // AckUploadVProgFile
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckUploadVProgFile_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckUploadVProgFile(data));
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

					AckUploadVProgFile_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckUploadVProgFile(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 1397310616: // AckIntermediateCode
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
				marshalling::operator>>(packet, data.result);
				marshalling::operator>>(packet, data.count);
				marshalling::operator>>(packet, data.index);
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
					get(props, "result", data.result);
					get(props, "count", data.count);
					get(props, "index", data.index);
					get(props, "data", data.data);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckIntermediateCode(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 4148808214: // AckRun
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
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckRun(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 3643391279: // AckOneStep
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
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckOneStep(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 1012496086: // AckResumeRun
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
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckResumeRun(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 2129545277: // AckBreak
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
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckBreak(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 114435221: // AckStop
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
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckStop(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 1658444570: // AckInput
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
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckInput(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 4206107288: // SyncVMInstruction
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
				marshalling::operator>>(packet, data.vmIdx);
				marshalling::operator>>(packet, data.indices);
				marshalling::operator>>(packet, data.cmps);
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
					get(props, "vmIdx", data.vmIdx);
					get(props, "indices", data.indices);
					get(props, "cmps", data.cmps);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMInstruction(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 3001685594: // SyncVMRegister
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
				marshalling::operator>>(packet, data.vmIdx);
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
					get(props, "vmIdx", data.vmIdx);
					get(props, "infoType", data.infoType);
					get(props, "reg", data.reg);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMRegister(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 3045798844: // SyncVMSymbolTable
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
				marshalling::operator>>(packet, data.vmIdx);
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
					get(props, "vmIdx", data.vmIdx);
					get(props, "start", data.start);
					get(props, "count", data.count);
					get(props, "symbol", data.symbol);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMSymbolTable(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 1348120458: // SyncVMOutput
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
				marshalling::operator>>(packet, data.vmIdx);
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
					get(props, "vmIdx", data.vmIdx);
					get(props, "output", data.output);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMOutput(data));
				} catch (...) {
					dbg::Logp("json packet parsing error\n");
				}
			}
		}
		break;

	case 1133387750: // AckHeartBeat
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
					dbg::Logp("json packet parsing error\n");
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



