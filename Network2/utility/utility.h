//
// 2021-09-08, jjuiddong
// network utility functions
//
#pragma once


namespace network2 
{

	iPacketHeader* GetPacketHeader(const int protocolId);
	iPacketHeader* GetPacketHeader(const ePacketFormat format);

}
