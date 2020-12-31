#include "stdafx.h"
#include "remotedbg_Protocol.h"
using namespace remotedbg;

//------------------------------------------------------------------------
// Protocol: AckOneStep
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::AckOneStep(netid targetId)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3643391279 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: AckDebugRun
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::AckDebugRun(netid targetId)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3424042780 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: AckBreak
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::AckBreak(netid targetId)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2129545277 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: AckTerminate
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::AckTerminate(netid targetId)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2796337704 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: UpdateInformation
//------------------------------------------------------------------------
void remotedbg::s2c_Protocol::UpdateInformation(netid targetId, const string &fileName, const string &vmName, const int &instIndex)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 4250626157 );
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
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3965635729 );
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
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1225501702 );
	packet << start;
	packet << count;
	packet << vmName;
	packet << symbols;
	packet << symbolVals;
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}



//------------------------------------------------------------------------
// Protocol: ReqOneStep
//------------------------------------------------------------------------
void remotedbg::c2s_Protocol::ReqOneStep(netid targetId)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2884814738 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqDebugRun
//------------------------------------------------------------------------
void remotedbg::c2s_Protocol::ReqDebugRun(netid targetId)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2143400932 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqBreak
//------------------------------------------------------------------------
void remotedbg::c2s_Protocol::ReqBreak(netid targetId)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 784411795 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: ReqTerminate
//------------------------------------------------------------------------
void remotedbg::c2s_Protocol::ReqTerminate(netid targetId)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3042238079 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}



