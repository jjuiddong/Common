
#include "stdafx.h"
#include "protocol.h"

using namespace network;


cProtocol::cProtocol()
	: m_packetLength(0)
{
	m_protocol[0] = 'N';
	m_protocol[1] = 'U';
	m_protocol[2] = 'L';
	m_protocol[3] = 'L';
}

cProtocol::cProtocol(const char protocol[4])
	: m_packetLength(0)
{
	memcpy(m_protocol, protocol, 4);
}


cProtocol::~cProtocol()
{
}


bool cProtocol::MakeHeader(const char *protocol, const int length)
{
	memcpy(m_protocol, protocol, 4);
	m_packetLength = length;
	return true;
}


// read protocol from memory
// byteSize : packet byte size from packet header
bool cProtocol::GetHeader(const BYTE *data, OUT int &byteSize)
{
	memcpy(m_protocol, data, 4);
	byteSize = (data[4] - '0') * 1000
		+ (data[5] - '0') * 100
		+ (data[6] - '0') * 10
		+ (data[7] - '0') * 1;
	m_packetLength = byteSize;
	return true;
}


// write protocol to dst memory
// packet length (ascii type), ex) "0085"
bool cProtocol::WriteHeader(BYTE *dst, const int packetSize)
{
	memcpy(dst, m_protocol, 4);

	const int size = GetHeaderSize() + packetSize;
	dst[4] = min(9, size / 1000) + '0';
	dst[5] = min(9, (size % 1000) / 100) + '0';
	dst[6] = min(9, (size % 100) / 10) + '0';
	dst[7] = min(9, size % 10) + '0';
	return true;
}


// return packet header size
int cProtocol::GetHeaderSize()
{
	return 8; // 8 bytes
}


int cProtocol::GetProtocolType()
{
	return *(int*)m_protocol;
}


int cProtocol::GetPacketLength()
{
	return m_packetLength;
}


bool cProtocol::IsValidPacket()
{
	const bool isValid = isalpha(m_protocol[0])
		&& isalpha(m_protocol[1])
		&& isalpha(m_protocol[2])
		&& isalpha(m_protocol[3])
		&& (m_packetLength > GetHeaderSize());
	return isValid;
}
