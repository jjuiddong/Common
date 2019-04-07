//
// 2019-01-08, jjuiddong
// Protocol Handler
//
// Network Packet Handler Interface
//
// Network Protocol Generation Compiler generates Protocol Handler as a source file.
//
#pragma once


namespace network2
{

	interface iProtocolHandler
	{
		// call before process packet
		virtual void BeforePacketProcess(const cPacket &packet) {}

		// call after process packet
		virtual void AfterPacketProcess(const cPacket &packet) {}
	};

}
