#include "pch.h"
#include "clustersync_Protocol.h"
using namespace clustersync;



//------------------------------------------------------------------------
// Protocol: SpawnRobot
//------------------------------------------------------------------------
void clustersync::c2s_Protocol::SpawnRobot(netid targetId, bool isBinary, const string &robotName, const string &modelName)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2663605951 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, robotName);
		marshalling::operator<<(packet, modelName);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "robotName", robotName);
			put(props, "modelName", modelName);
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
// Protocol: RemoveRobot
//------------------------------------------------------------------------
void clustersync::c2s_Protocol::RemoveRobot(netid targetId, bool isBinary, const string &robotName)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 27410734 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, robotName);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "robotName", robotName);
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
// Protocol: RobotWorkFinish
//------------------------------------------------------------------------
void clustersync::c2s_Protocol::RobotWorkFinish(netid targetId, bool isBinary, const string &robotName, const uint &taskId, const uint &totalWorkCount, const uint &finishWorkCount)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2061430798 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, robotName);
		marshalling::operator<<(packet, taskId);
		marshalling::operator<<(packet, totalWorkCount);
		marshalling::operator<<(packet, finishWorkCount);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "robotName", robotName);
			put(props, "taskId", taskId);
			put(props, "totalWorkCount", totalWorkCount);
			put(props, "finishWorkCount", finishWorkCount);
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
// Protocol: RealtimeRobotState
//------------------------------------------------------------------------
void clustersync::c2s_Protocol::RealtimeRobotState(netid targetId, bool isBinary, const string &robotName, const BYTE &state1, const BYTE &state2, const BYTE &state3, const BYTE &state4, const Vector3 &pos, const Vector3 &dir)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1079519326 );
	packet.SetPacketOption(0x01, (uint)isBinary);
	if (isBinary)
	{
		// marshaling binary
		packet.Alignment4(); // set 4byte alignment
		marshalling::operator<<(packet, robotName);
		marshalling::operator<<(packet, state1);
		marshalling::operator<<(packet, state2);
		marshalling::operator<<(packet, state3);
		marshalling::operator<<(packet, state4);
		marshalling::operator<<(packet, pos);
		marshalling::operator<<(packet, dir);
		packet.EndPack();
		GetNode()->Send(targetId, packet);
	}
	else
	{
		// marshaling json
		using boost::property_tree::ptree;
		ptree props;
		try {
			put(props, "robotName", robotName);
			put(props, "state1", state1);
			put(props, "state2", state2);
			put(props, "state3", state3);
			put(props, "state4", state4);
			put(props, "pos", pos);
			put(props, "dir", dir);
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



