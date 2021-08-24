#include "pch.h"
#include "editor_Protocol.h"
using namespace editor;

//------------------------------------------------------------------------
// Protocol: AckRunSimulation
//------------------------------------------------------------------------
void editor::s2c_Protocol::AckRunSimulation(netid targetId, bool isBinary, const string &projectName, const string &mapFileName, const string &pathFileName, const vector<string> &nodeFileNames, const string &taskName, const int &result)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 207562622 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, projectName);
		marshalling::operator<<(packet, mapFileName);
		marshalling::operator<<(packet, pathFileName);
		marshalling::operator<<(packet, nodeFileNames);
		marshalling::operator<<(packet, taskName);
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
			put(props, "projectName", projectName);
			put(props, "mapFileName", mapFileName);
			put(props, "pathFileName", pathFileName);
			put(props, "nodeFileNames", nodeFileNames);
			put(props, "taskName", taskName);
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
// Protocol: AckRunProject
//------------------------------------------------------------------------
void editor::s2c_Protocol::AckRunProject(netid targetId, bool isBinary, const string &projectName, const int &result)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1279498047 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, projectName);
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
			put(props, "projectName", projectName);
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
// Protocol: AckStopSimulation
//------------------------------------------------------------------------
void editor::s2c_Protocol::AckStopSimulation(netid targetId, bool isBinary, const int &result)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 4276051529 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
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
// Protocol: ReqRunSimulation
//------------------------------------------------------------------------
void editor::c2s_Protocol::ReqRunSimulation(netid targetId, bool isBinary, const string &projectName, const string &mapFileName, const string &pathFileName, const vector<string> &nodeFileNames, const string &taskName)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2415569453 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, projectName);
		marshalling::operator<<(packet, mapFileName);
		marshalling::operator<<(packet, pathFileName);
		marshalling::operator<<(packet, nodeFileNames);
		marshalling::operator<<(packet, taskName);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "projectName", projectName);
			put(props, "mapFileName", mapFileName);
			put(props, "pathFileName", pathFileName);
			put(props, "nodeFileNames", nodeFileNames);
			put(props, "taskName", taskName);
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
// Protocol: ReqRunProject
//------------------------------------------------------------------------
void editor::c2s_Protocol::ReqRunProject(netid targetId, bool isBinary, const string &projectName)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2272961170 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, projectName);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "projectName", projectName);
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
// Protocol: ReqStopSimulation
//------------------------------------------------------------------------
void editor::c2s_Protocol::ReqStopSimulation(netid targetId, bool isBinary)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3081823622 );
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



