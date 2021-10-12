//
// 2021-09-08, jjuiddong
// network utility functions
//
#pragma once


namespace network2 
{

	void DisplayPacket(const string &firstStr, const cPacket &packet
		, const int logLevel = 0);
	void GetPacketString(const cPacket &packet, OUT string &out);
	StrId GetPacketName(const cPacket &packet);
	ePacketFormat GetPacketFormat(const cPacket &packet);
	ePacketFormat GetPacketFormat(const int protocolId);
	iPacketHeader* GetPacketHeader(const int protocolId);
	iPacketHeader* GetPacketHeader(const ePacketFormat format);

	void DisplayPacketCleanup();

}
