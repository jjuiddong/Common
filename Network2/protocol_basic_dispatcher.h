//
// 2019-01-09, jjuiddong
// Basic Protocol Dispatcher
//
// Process Basic Packets Dispatcher
//	- Connect, Disconnect, Accept
//
#pragma once


namespace basic_protocol
{
	using namespace network2;
	using namespace marshalling;
	static const int Dispatcher_ID = 0;

	// Server Basic Protocol Dispatcher
	class ServerDispatcher
	{
	public:
		void Dispatch(cPacket &packet, cTcpServer *svr);
	};

	// Client Basic Protocol Dispatcher
	class ClientDispatcher
	{
	public:
		void Dispatch(cPacket &packet, cTcpClient *client);
	};

	// Server Basic Protocol Dispatcher
	class UdpServerDispatcher
	{
	public:
		void Dispatch(cPacket &packet, cUdpServer *svr) {} // nothing
	};

	// Web Client Basic Protocol Dispatcher
	class WebClientDispatcher
	{
	public:
		void Dispatch(cPacket &packet, cWebClient *client);
	};

}
