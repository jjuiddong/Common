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

	// TCP Server Basic Protocol Dispatcher
	class ServerDispatcher
	{
	public:
		void Dispatch(cPacket &packet, cTcpServer *svr);
	};

	// TCP Client Basic Protocol Dispatcher
	class ClientDispatcher
	{
	public:
		void Dispatch(cPacket &packet, cTcpClient *client);
	};

	// UDP Server Basic Protocol Dispatcher
	class UdpServerDispatcher
	{
	public:
		void Dispatch(cPacket &packet, cUdpServer *svr);
	};

	// UDP Server2 Basic Protocol Dispatcher
	class UdpServer2Dispatcher
	{
	public:
		void Dispatch(cPacket& packet, cUdpServer2* svr);
	};

	// UDP Client2 Basic Protocol Dispatcher
	class UdpClient2Dispatcher
	{
	public:
		void Dispatch(cPacket& packet, cUdpClient2* client);
	};

	// Web Server Basic Protocol Dispatcher
	class WebServerDispatcher
	{
	public:
		void Dispatch(cPacket &packet, cWebServer *svr);
	};

	// Web Client Basic Protocol Dispatcher
	class WebClientDispatcher
	{
	public:
		void Dispatch(cPacket &packet, cWebClient *client);
	};

}
