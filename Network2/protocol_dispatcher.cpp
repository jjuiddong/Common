
#include "stdafx.h"
#include "protocol_dispatcher.h"

using namespace network2;

map<int, cProtocolDispatcher*> *cProtocolDispatcher::s_dispatchers = nullptr;
map<int, iPacketHeader*> *cProtocolDispatcher::s_packetHeaders = nullptr;

cProtocolDispatcher::cProtocolDispatcher(const int id
	, const ePacketFormat format //= ePacketFormat::BINARY
)
	: m_id(id) 
	, m_format(format)
	, m_packet(NULL)
{
}

cProtocolDispatcher::~cProtocolDispatcher() 
{
}


void cProtocolDispatcher::SetCurrentDispatchPacket(cPacket *pPacket) 
{ 
	m_packet = pPacket; 
}
