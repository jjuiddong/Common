
#include "stdafx.h"
#include "networknode.h"

using namespace network2;

// default packet header is binary format
// to avoid null packet header
static cPacketHeader s_binPacketHeader;

cNetworkNode::cNetworkNode(const StrId &name //= "NetNode"
	, const int logId //= -1
)
	: cSession(common::GenerateId(), name, logId)
{
}

cNetworkNode::~cNetworkNode()
{
	Close();
}


void cNetworkNode::RegisterProtocol(iProtocol *protocol)
{
	protocol->SetNode(this);
}


void cNetworkNode::Close()
{
	__super::Close();
}


// Add ProtocolHandler
bool cNetworkNode::AddProtocolHandler(iProtocolHandler *handler)
{
	for (auto &p : m_protocolHandlers)
		if (p == handler)
			return false; // already exist
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


// default send
int cNetworkNode::SendPacket(const SOCKET sock, const cPacket &packet)
{
	const int result = send(sock, (const char*)packet.m_data, packet.GetPacketSize(), 0);
	return result;
}


// default sendto
int cNetworkNode::SendToPacket(const SOCKET sock, const sockaddr_in &sockAddr
	, const cPacket &packet)
{
	const int result = sendto(sock, (const char*)packet.m_data, packet.GetPacketSize()
		, 0, (struct sockaddr*) &sockAddr, sizeof(sockAddr));
	return result;
}
