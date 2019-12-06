#include "basic_Protocol.h"
using namespace basic;

//------------------------------------------------------------------------
// Protocol: func1
//------------------------------------------------------------------------
void basic::s2c_Protocol::func1(netid targetId)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3587614684 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: func2
//------------------------------------------------------------------------
void basic::s2c_Protocol::func2(netid targetId, const std::string &str)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 2592878926 );
	packet << str;
	AddDelimeter(packet);
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: func3
//------------------------------------------------------------------------
void basic::s2c_Protocol::func3(netid targetId, const float &value)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 3387225200 );
	packet << value;
	AddDelimeter(packet);
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: func4
//------------------------------------------------------------------------
void basic::s2c_Protocol::func4(netid targetId)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 4124054319 );
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: func5
//------------------------------------------------------------------------
void basic::s2c_Protocol::func5(netid targetId, const string &name, const vector<int> &ar)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 622789402 );
	packet << name;
	AddDelimeter(packet);
	packet << ar;
	AddDelimeter(packet);
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: func6
//------------------------------------------------------------------------
void basic::s2c_Protocol::func6(netid targetId, const vector<int> &ar)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1506470321 );
	packet << ar;
	AddDelimeter(packet);
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}



//------------------------------------------------------------------------
// Protocol: func5
//------------------------------------------------------------------------
void basic::c2s_Protocol::func5(netid targetId, const std::string &str)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 622789402 );
	packet << str;
	AddDelimeter(packet);
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}

//------------------------------------------------------------------------
// Protocol: func6
//------------------------------------------------------------------------
void basic::c2s_Protocol::func6(netid targetId, const float &value)
{
	cPacket packet(m_node->GetPacketHeader());
	packet.SetProtocolId( GetId() );
	packet.SetPacketId( 1506470321 );
	packet << value;
	AddDelimeter(packet);
	packet.EndPack();
	GetNode()->Send(targetId, packet);
}



