#include "stdafx.h"
#include "remotedbg2_Protocol.h"
using namespace remotedbg2;

cPacketHeaderJson remotedbg2::r2h_Protocol::s_packetHeader;
//------------------------------------------------------------------------
// Protocol: UploadIntermediateCode
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::UploadIntermediateCode(netid targetId, bool isBinary, const int &itprId, const string &code)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1418562193 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, code);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "code", code);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqIntermediateCode
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqIntermediateCode(netid targetId, bool isBinary, const int &itprId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1644585100 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqRun
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqRun(netid targetId, bool isBinary, const int &itprId, const string &runType)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 923151823 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, runType);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "runType", runType);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqOneStep
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqOneStep(netid targetId, bool isBinary, const int &itprId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2884814738 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqResumeRun
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqResumeRun(netid targetId, bool isBinary, const int &itprId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 742173167 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqBreak
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqBreak(netid targetId, bool isBinary, const int &itprId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 784411795 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqBreakPoint
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqBreakPoint(netid targetId, bool isBinary, const int &itprId, const bool &enable, const uint &id)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2487089996 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, enable);
		marshalling::operator<<(packet, id);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "enable", enable);
			put(props, "id", id);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqStop
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqStop(netid targetId, bool isBinary, const int &itprId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1453251868 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqInput
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqInput(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const string &eventName)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3140751413 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, eventName);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "eventName", eventName);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqEvent
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqEvent(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const string &eventName)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 186222094 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, eventName);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "eventName", eventName);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqStepDebugType
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqStepDebugType(netid targetId, bool isBinary, const int &stepDbgType)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3084593987 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, stepDbgType);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "stepDbgType", stepDbgType);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqDebugInfo
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqDebugInfo(netid targetId, bool isBinary, const vector<int> &itprIds)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2166551586 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprIds);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprIds", itprIds);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqVariableInfo
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqVariableInfo(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const string &varName)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 4127584998 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, varName);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "varName", varName);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: ReqHeartBeat
//------------------------------------------------------------------------
void remotedbg2::r2h_Protocol::ReqHeartBeat(netid targetId, bool isBinary)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2532286881 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}



cPacketHeaderJson remotedbg2::h2r_Protocol::s_packetHeader;
//------------------------------------------------------------------------
// Protocol: Welcome
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::Welcome(netid targetId, bool isBinary, const string &msg)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1281093745 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, msg);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "msg", msg);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckUploadIntermediateCode
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckUploadIntermediateCode(netid targetId, bool isBinary, const int &itprId, const int &result)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 4005257575 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, result);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "result", result);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckIntermediateCode
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckIntermediateCode(netid targetId, bool isBinary, const BYTE &itprId, const BYTE &result, const BYTE &count, const BYTE &index, const uint &totalBufferSize, const vector<BYTE> &data)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1397310616 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, result);
		marshalling::operator<<(packet, count);
		marshalling::operator<<(packet, index);
		marshalling::operator<<(packet, totalBufferSize);
		marshalling::operator<<(packet, data);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "result", result);
			put(props, "count", count);
			put(props, "index", index);
			put(props, "totalBufferSize", totalBufferSize);
			put(props, "data", data);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckRun
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckRun(netid targetId, bool isBinary, const int &itprId, const int &result)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 4148808214 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, result);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "result", result);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckOneStep
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckOneStep(netid targetId, bool isBinary, const int &itprId, const int &result)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3643391279 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, result);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "result", result);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckResumeRun
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckResumeRun(netid targetId, bool isBinary, const int &itprId, const int &result)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1012496086 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, result);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "result", result);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckBreak
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckBreak(netid targetId, bool isBinary, const int &itprId, const int &result)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2129545277 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, result);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "result", result);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckBreakPoint
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckBreakPoint(netid targetId, bool isBinary, const int &itprId, const bool &enable, const uint &id, const int &result)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2045074648 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, enable);
		marshalling::operator<<(packet, id);
		marshalling::operator<<(packet, result);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "enable", enable);
			put(props, "id", id);
			put(props, "result", result);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckStop
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckStop(netid targetId, bool isBinary, const int &itprId, const int &result)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 114435221 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, result);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "result", result);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckInput
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckInput(netid targetId, bool isBinary, const int &itprId, const int &result)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1658444570 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, result);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "result", result);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckEvent
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckEvent(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const string &eventName, const int &result)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1906481345 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, eventName);
		marshalling::operator<<(packet, result);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "eventName", eventName);
			put(props, "result", result);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckStepDebugType
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckStepDebugType(netid targetId, bool isBinary, const int &stepDbgType, const int &result)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 4225702489 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, stepDbgType);
		marshalling::operator<<(packet, result);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "stepDbgType", stepDbgType);
			put(props, "result", result);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckDebugInfo
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckDebugInfo(netid targetId, bool isBinary, const vector<int> &itprIds, const int &result)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 4276104084 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprIds);
		marshalling::operator<<(packet, result);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprIds", itprIds);
			put(props, "result", result);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: SyncVMInstruction
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::SyncVMInstruction(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const vector<ushort> &indices)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 4206107288 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, indices);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "indices", indices);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: SyncVMRegister
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::SyncVMRegister(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const int &infoType, const script::cVirtualMachine::sRegister &reg)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3001685594 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, infoType);
		marshalling::operator<<(packet, reg);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "infoType", infoType);
			put(props, "reg", reg);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: SyncVMSymbolTable
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::SyncVMSymbolTable(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const uint &start, const uint &count, const vector<script::sSyncSymbol> &symbol)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3045798844 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, start);
		marshalling::operator<<(packet, count);
		marshalling::operator<<(packet, symbol);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "start", start);
			put(props, "count", count);
			put(props, "symbol", symbol);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: SyncVMOutput
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::SyncVMOutput(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const string &output)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1348120458 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, output);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "output", output);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: SyncVMWidgets
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::SyncVMWidgets(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const string &widgetName)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 323195839 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, widgetName);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "widgetName", widgetName);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: SyncVMArray
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::SyncVMArray(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const string &varName, const uint &startIdx, const vector<variant_t> &array)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1209241191 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, varName);
		marshalling::operator<<(packet, startIdx);
		marshalling::operator<<(packet, array);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "varName", varName);
			put(props, "startIdx", startIdx);
			put(props, "array", array);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: SyncVMArrayBool
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::SyncVMArrayBool(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const string &varName, const uint &startIdx, const vector<bool> &array)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3278867969 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, varName);
		marshalling::operator<<(packet, startIdx);
		marshalling::operator<<(packet, array);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "varName", varName);
			put(props, "startIdx", startIdx);
			put(props, "array", array);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: SyncVMArrayNumber
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::SyncVMArrayNumber(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const string &varName, const uint &startIdx, const vector<float> &array)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3822230413 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, varName);
		marshalling::operator<<(packet, startIdx);
		marshalling::operator<<(packet, array);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "varName", varName);
			put(props, "startIdx", startIdx);
			put(props, "array", array);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: SyncVMArrayString
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::SyncVMArrayString(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const string &varName, const uint &startIdx, const vector<string> &array)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2291689449 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, itprId);
		marshalling::operator<<(packet, vmIdx);
		marshalling::operator<<(packet, varName);
		marshalling::operator<<(packet, startIdx);
		marshalling::operator<<(packet, array);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "itprId", itprId);
			put(props, "vmIdx", vmIdx);
			put(props, "varName", varName);
			put(props, "startIdx", startIdx);
			put(props, "array", array);
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}

//------------------------------------------------------------------------
// Protocol: AckHeartBeat
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckHeartBeat(netid targetId, bool isBinary)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1133387750 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			stringstream ss;
			boost::property_tree::write_json(ss, props);
			packet << ss.str();
			packet.EndPack();
			GetNode()->Send(targetId, packet);
		} catch (...) {
			dbg::Logp("json packet maker error\n");
		}
	}
}



