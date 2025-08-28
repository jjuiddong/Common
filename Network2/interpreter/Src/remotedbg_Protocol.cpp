#include "stdafx.h"
#include "remotedbg_Protocol.h"
using namespace remotedbg;

cPacketHeader remotedbg::s2c_Protocol::s_packetHeader;
//------------------------------------------------------------------------
// Protocol: AckOneStep
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::AckOneStep(netid targetId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3508334431 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: AckDebugRun
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::AckDebugRun(netid targetId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2217004160 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: AckBreak
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::AckBreak(netid targetId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3655568643 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: AckTerminate
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::AckTerminate(netid targetId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3075729906 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: UpdateInformation
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::UpdateInformation(netid targetId, const string &fileName, const string &vmName, const int &instIndex)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1732498169 );
	packet << fileName;
	packet << vmName;
	packet << instIndex;
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: UpdateState
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::UpdateState(netid targetId, const string &vmName, const int &instIndex)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 566400524 );
	packet << vmName;
	packet << instIndex;
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: UpdateSymbolTable
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::UpdateSymbolTable(netid targetId, const int &start, const int &count, const string &vmName, const vector<string> &symbols, const vector<int> &symbolVals)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2443270683 );
	packet << start;
	packet << count;
	packet << vmName;
	packet << symbols;
	packet << symbolVals;
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}



cPacketHeader remotedbg::c2s_Protocol::s_packetHeader;
//------------------------------------------------------------------------
// Protocol: ReqOneStep
//------------------------------------------------------------------------
void remotedbg::c2s_Protocol::ReqOneStep(netid targetId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 250175983 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqDebugRun
//------------------------------------------------------------------------
void remotedbg::c2s_Protocol::ReqDebugRun(netid targetId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2556419112 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqBreak
//------------------------------------------------------------------------
void remotedbg::c2s_Protocol::ReqBreak(netid targetId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1318526848 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqTerminate
//------------------------------------------------------------------------
void remotedbg::c2s_Protocol::ReqTerminate(netid targetId)
{
	cPacket packet(&s_packetHeader);
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2259737201 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}



