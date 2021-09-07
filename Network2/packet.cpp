
#include "stdafx.h"
#include "packet.h"

using namespace network2;
using namespace marshalling;

cPacket::cPacket()
	: m_packetHeader(NULL)
	, m_is4Align(false)
	, m_writeIdx(0)
	, m_readIdx(0)
	, m_lastDelim(NULL)
	, m_emptyData(false)
	, m_data(m_buffer)
	, m_bufferSize(DEFAULT_PACKETSIZE)
{
}

cPacket::cPacket(iPacketHeader *packetHeader)
	: m_packetHeader(packetHeader)
	, m_writeIdx(packetHeader? packetHeader->GetHeaderSize() : 0)
	, m_readIdx(packetHeader? packetHeader->GetHeaderSize() : 0)
	, m_lastDelim(NULL)
	, m_emptyData(false)
	, m_data(m_buffer)
	, m_bufferSize(DEFAULT_PACKETSIZE)
{
}

cPacket::cPacket(iPacketHeader *packetHeader, const BYTE *src, const int byteSize)
	: m_packetHeader(packetHeader)
	, m_readIdx(packetHeader? packetHeader->GetHeaderSize() : 0)
	, m_lastDelim(NULL)
	, m_emptyData(false)
	, m_data(m_buffer)
	, m_bufferSize(DEFAULT_PACKETSIZE)
{
	m_writeIdx = min((uint)byteSize, sizeof(m_buffer));
	memcpy_s(m_data, sizeof(m_buffer), src, byteSize);
}

cPacket::cPacket(const cPacket &rhs)
	: m_data(m_buffer)
	, m_bufferSize(DEFAULT_PACKETSIZE)
{
	operator=(rhs);
}

cPacket::~cPacket()
{
}


// call before write
void cPacket::InitWrite()
{
	RET(!m_packetHeader);
	m_writeIdx = m_packetHeader->GetHeaderSize();
}


// call before read
void cPacket::InitRead()
{
	RET(!m_packetHeader);
	m_readIdx = m_packetHeader->GetHeaderSize();
}


// initialize read/write cursor
void cPacket::Initialize()
{
	RET(!m_packetHeader);
	m_readIdx = m_packetHeader->GetHeaderSize();
	m_writeIdx = m_packetHeader->GetHeaderSize();
}


// 4byte alignment, websocket binary packet
// int, uint, float, double type, offset is always *4 multiple
void cPacket::Alignment4()
{
	m_is4Align = true;
}


// call before send packet
void cPacket::EndPack()
{
	RET(!m_packetHeader);
	if (m_writeIdx < m_bufferSize)
		m_writeIdx += m_packetHeader->SetPacketTerminator(&m_data[m_writeIdx], m_bufferSize - m_writeIdx);

	SetPacketSize(GetWriteSize());
}


// shallow clone
void cPacket::ShallowCopy(const cPacket &packet)
{
	m_packetHeader = packet.m_packetHeader;
	m_sndId = packet.m_sndId;
	m_is4Align = packet.m_is4Align;
	m_readIdx = packet.m_readIdx;
	m_writeIdx = packet.m_writeIdx;
	m_lastDelim = packet.m_lastDelim;
	m_emptyData = packet.m_emptyData;
	m_bufferSize = packet.m_bufferSize;
	m_data = packet.m_data;
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
		memcpy(m_data, rhs.m_data, sizeof(rhs.m_buffer));
	}
	return *this;
}



void cPacket::SetProtocolId(const int protocolId) {
	RET(!m_packetHeader);
	m_packetHeader->SetProtocolId(m_data, protocolId);
}
void cPacket::SetPacketId(const int packetId) {
	RET(!m_packetHeader);
	m_packetHeader->SetPacketId(m_data, packetId);
}
void cPacket::SetPacketSize(const short packetSize) {
	RET(!m_packetHeader);
	m_packetHeader->SetPacketLength(m_data, packetSize);
}
void cPacket::SetPacketOption(const uint mask, const uint option) {
	RET(!m_packetHeader);
	m_packetHeader->SetOptionBits(m_data, mask, option);
}
void cPacket::SetSenderId(const netid netId) {
	m_sndId = netId;
}
int cPacket::GetProtocolId() const {
	return m_packetHeader->GetProtocolId(m_data);
}
uint cPacket::GetPacketId() const {
	RETV(!m_packetHeader, 0);
	return m_packetHeader->GetPacketId(m_data);
}
uint cPacket::GetPacketSize() const {
	RETV(!m_packetHeader, 0);
	return (uint)min((uint)m_bufferSize, m_packetHeader->GetPacketLength(m_data));
}
uint cPacket::GetPacketOption(const uint mask) {
	RETV(!m_packetHeader, 0);
	return m_packetHeader->GetOptionBits(m_data, mask);
}
int cPacket::GetWriteSize() const {
	return m_writeIdx;
}
int cPacket::GetSenderId() const {
	return m_sndId;
}

int cPacket::GetHeaderSize() const {
	RETV(!m_packetHeader, 0);
	return m_packetHeader->GetHeaderSize();
}

bool cPacket::IsValid()
{
	RETV(!m_packetHeader, false);
	return m_packetHeader->IsValidPacket(m_data);
}

// m_readIdx 4 byte alignment, to parsing JavaScript ArrayBuffer
void cPacket::Read4ByteAlign()
{
	m_readIdx = (m_readIdx + 0x03) & (~0x03);
}

// m_writeIdx 4 byte alignment, to send webserver
void cPacket::Write4ByteAlign()
{
	m_writeIdx = (m_writeIdx + 0x03) & (~0x03);
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
