//
// 2019-02-22, jjuiddong
// Protocol Display Utillity
//
#pragma once


namespace network2
{

	void DisplayPacket(const Str128 &firstStr, const cPacket &packet, const int logLevel=0);

	void GetPacketString(const cPacket &packet, OUT string &out );
	
	StrId GetPacketName(const cPacket &packet);

	void DisplayPacketCleanup();

}
