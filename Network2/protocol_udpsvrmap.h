//
// 2021-09-30, jjuiddong
// cUdpServerMap basic protocol
//	- cUdpServerMap Error Handling Protocol
//		- override udpsvrmap::ProtocolHanlder interface
//
#pragma once


namespace udpsvrmap
{

	using namespace network2;
	using namespace marshalling;
	static const int dispatcher_ID = 11; // reserved protocol id

	struct Close_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string serverName;
	};
	struct Error_Packet {
		cProtocolDispatcher *pdispatcher;
		netid senderId;
		string serverName;
		int errCode;
	};


	// Protocol Dispatcher
	class Dispatcher : public network2::cProtocolDispatcher
	{
	public:
		Dispatcher();
	protected:
		virtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;
		static cPacketHeader s_packetHeader;
	};
	static Dispatcher g_udpsvrmap_Dispatcher;


	// ProtocolHandler
	class ProtocolHandler : virtual public network2::iProtocolHandler
	{
	public:
		friend class Dispatcher;
		ProtocolHandler() { m_format = ePacketFormat::BINARY; }
		virtual bool Close(udpsvrmap::Close_Packet &packet) { return true; }
		virtual bool Error(udpsvrmap::Error_Packet &packet) { return true; }
	};


	cPacket SendClose(network2::cNetworkNode *node);
	cPacket SendError(network2::cNetworkNode *node, int errCode);

}
