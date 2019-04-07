
#include "stdafx.h"
#include "networknode.h"

using namespace network2;


cNetworkNode::cNetworkNode(
	iPacketHeader *packetHeader
	, const StrId &name //= "NetNode"
	, const bool isPacketLog //= false
)
	: cSession(common::GenerateId(), name, isPacketLog)
	, m_packetHeader(packetHeader)
{
}

cNetworkNode::~cNetworkNode()
{
	Close();

	SAFE_DELETE(m_packetHeader);
}


void cNetworkNode::RegisterProtocol(iProtocol *protocol)
{
	protocol->SetNode(this);
}


void cNetworkNode::Close()
{
	cSession::Close();
}


// Add ProtocolHandler
bool cNetworkNode::AddProtocolHandler(iProtocolHandler *handler)
{
	for (auto &p : m_protocolHandlers)
		if (p == handler)
			return false;
	m_protocolHandlers.push_back(handler);
	return true;
}


// Remove ProtocolHandler
bool cNetworkNode::RemoveProtocolHandler(iProtocolHandler *handler)
{
	for (auto &p : m_protocolHandlers)
	{
		if (p == handler)
		{
			common::removevector(m_protocolHandlers, handler);
			return true;
		}
	}
	return false;
}


// return protocolhandlers
vector<iProtocolHandler*>& cNetworkNode::GetProtocolHandlers()
{
	return m_protocolHandlers;
}


// return Protocol Packet Header
iPacketHeader* cNetworkNode::GetPacketHeader()
{
	return m_packetHeader;
}
