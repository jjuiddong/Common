
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
	, m_packetHeader(&s_binPacketHeader)
{
}

cNetworkNode::~cNetworkNode()
{
	Close();

	if (m_packetHeader != &s_binPacketHeader)
		SAFE_DELETE(m_packetHeader);
}


void cNetworkNode::RegisterProtocol(iProtocol *protocol)
{
	protocol->SetNode(this);
	CreatePacketHeader(protocol->m_format);
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
			return false; // already exist
	m_protocolHandlers.push_back(handler);

	CreatePacketHeader(handler->m_format);
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


// create packet header
bool cNetworkNode::CreatePacketHeader(const ePacketFormat format)
{
	if (m_packetHeader != &s_binPacketHeader)
		return true; // already created!

	switch (format)
	{
	case ePacketFormat::BINARY: m_packetHeader = new cPacketHeader(); break;
	case ePacketFormat::ASCII: m_packetHeader = new cPacketHeaderAscii(); break;
	case ePacketFormat::JSON: m_packetHeader = new cPacketHeaderJson(); break;
	case ePacketFormat::FREE: m_packetHeader = new cPacketHeaderNoFormat(); break;
	default: return false; // error type
	}
	return true;
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
