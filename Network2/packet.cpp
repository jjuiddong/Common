
#include "stdafx.h"
#include "packet.h"

using namespace network2;
using namespace marshalling;

cPacket::cPacket()
	: m_packetHeader(NULL)
	, m_writeIdx(0)
	, m_readIdx(0)
	, m_lastDelim(NULL)
	, m_emptyData(false)
{
}

cPacket::cPacket(iPacketHeader *packetHeader)
	: m_packetHeader(packetHeader)
	, m_writeIdx(packetHeader->GetHeaderSize())
	, m_readIdx(packetHeader->GetHeaderSize())
	, m_lastDelim(NULL)
	, m_emptyData(false)
{
}

cPacket::cPacket(iPacketHeader *packetHeader, const BYTE *src, const int byteSize)
	: m_packetHeader(packetHeader)
	, m_readIdx(packetHeader->GetHeaderSize())
	, m_lastDelim(NULL)
	, m_emptyData(false)
{
	m_writeIdx = min(byteSize, sizeof(m_data));
	memcpy_s(m_data, sizeof(m_data), src, byteSize);
}

cPacket::cPacket(const cPacket &rhs)
{
	operator=(rhs);
}

cPacket::~cPacket()
{
}


// call before read
void cPacket::InitRead()
{
	m_readIdx = m_packetHeader->GetHeaderSize();
}

// call before send packet
void cPacket::EndPack()
{
	if (m_writeIdx < DEFAULT_PACKETSIZE)
		m_writeIdx += m_packetHeader->SetPacketTerminator(&m_data[m_writeIdx], DEFAULT_PACKETSIZE - m_writeIdx);

	SetPacketSize(GetWriteSize());
}


cPacket& cPacket::operator=(const cPacket &rhs)
{
	if (this != &rhs)
	{
		m_packetHeader = rhs.m_packetHeader;
		m_sndId = rhs.m_sndId;
		m_readIdx = rhs.m_readIdx;
		m_writeIdx = rhs.m_writeIdx;
		m_lastDelim = rhs.m_lastDelim;
		m_emptyData = rhs.m_emptyData;
		memcpy(m_data, rhs.m_data, sizeof(rhs.m_data));
	}
	return *this;
}



void cPacket::SetProtocolId(const int protocolId) {
	m_packetHeader->SetProtocolId(m_data, protocolId);
}
void cPacket::SetPacketId(const int packetId) {
	m_packetHeader->SetPacketId(m_data, packetId);
}
void cPacket::SetPacketSize(const short packetSize) {
	m_packetHeader->SetPacketLength(m_data, packetSize);
}
void cPacket::SetSenderId(const netid netId) {
	m_sndId = netId;
}
int cPacket::GetProtocolId() const {
	return m_packetHeader->GetProtocolId(m_data);
}
uint cPacket::GetPacketId() const {
	return m_packetHeader->GetPacketId(m_data);
}
uint cPacket::GetPacketSize() const {
	return (uint)min(DEFAULT_PACKETSIZE, (int)m_packetHeader->GetPacketLength(m_data));
}
int cPacket::GetWriteSize() const {
	return m_writeIdx;
}
int cPacket::GetSenderId() const {
	return m_sndId;
}

int cPacket::GetHeaderSize() const {
	return m_packetHeader->GetHeaderSize();
}


bool cPacket::IsValid()
{
	return m_packetHeader->IsValidPacket(m_data);
}


//--------------------------------------------------------------------------------
// Basic Packet

// Global Reserved Packet
// Disconnect 패킷을 리턴한다.
// netid: disconnect 된 netid
// binary marshalling
cPacket network2::DisconnectPacket(cNetworkNode *node, netid disconnectId)
{
	cPacket packet(node->GetPacketHeader());
	packet.SetSenderId(SERVER_NETID);
	packet.SetProtocolId(0); // basic_protocol
	packet.SetPacketId(PACKETID_DISCONNECT);
	packet << disconnectId;
	packet.EndPack();
	return packet;
}


// Client Disconnect 패킷을 리턴한다.
// netid: disconnect 된 netid
// binary marshalling
cPacket network2::ClientDisconnectPacket(cNetworkNode *node, netid disconnectId)
{
	cPacket packet(node->GetPacketHeader());
	packet.SetSenderId(SERVER_NETID);
	packet.SetProtocolId(0); // basic_protocol
	packet.SetPacketId(PACKETID_CLIENT_DISCONNECT);
	packet << disconnectId;
	packet.EndPack();
	return packet;
}


// return the AcceptPacket
// binary marshalling
cPacket network2::AcceptPacket(cNetworkNode *node, SOCKET acceptSocket, const string &clientIP, int port)
{
	cPacket packet(node->GetPacketHeader());
	packet.SetSenderId(SERVER_NETID);
	packet.SetProtocolId(0); // basic_protocol
	packet.SetPacketId(PACKETID_ACCEPT);
	packet << acceptSocket;
	packet << clientIP;
	packet << port;
	packet.EndPack();
	return packet;
}
