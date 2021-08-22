#include "pch.h"
#include "remotedbg2_Protocol.h"
using namespace remotedbg2;

//------------------------------------------------------------------------
// Protocol: AckUploadIntermediateCode
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckUploadIntermediateCode(netid targetId, bool isBinary, const int &itprId, const int &result)
{
	cPacket packet(m_node->GetPacketHeader());
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
void remotedbg2::h2r_Protocol::AckIntermediateCode(netid targetId, bool isBinary, const int &itprId, const int &result, const uint &count, const uint &index, const vector<BYTE> &data)
{
	cPacket packet(m_node->GetPacketHeader());
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
	cPacket packet(m_node->GetPacketHeader());
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
	cPacket packet(m_node->GetPacketHeader());
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
	cPacket packet(m_node->GetPacketHeader());
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
	cPacket packet(m_node->GetPacketHeader());
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
	cPacket packet(m_node->GetPacketHeader());
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
	cPacket packet(m_node->GetPacketHeader());
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
	cPacket packet(m_node->GetPacketHeader());
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
// Protocol: AckStepDebugType
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckStepDebugType(netid targetId, bool isBinary, const int &stepDbgType, const int &result)
{
	cPacket packet(m_node->GetPacketHeader());
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
// Protocol: SyncVMInstruction
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::SyncVMInstruction(netid targetId, bool isBinary, const int &itprId, const int &vmIdx, const vector<ushort> &indices)
{
	cPacket packet(m_node->GetPacketHeader());
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
	cPacket packet(m_node->GetPacketHeader());
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
	cPacket packet(m_node->GetPacketHeader());
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
	cPacket packet(m_node->GetPacketHeader());
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
// Protocol: AckHeartBeat
//------------------------------------------------------------------------
void remotedbg2::h2r_Protocol::AckHeartBeat(netid targetId, bool isBinary)
{
	cPacket packet(m_node->GetPacketHeader());
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



