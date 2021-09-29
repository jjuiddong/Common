//
// 2019-07-01, jjuiddong 
// mulitple udp server managing class
//
// 2021-09-27
//	- add UdpServer Bind Error, Close Handling Protocol
//	- thread mode on/off
//
#pragma once


namespace network2
{

	class cUdpServerMap
	{
	public:
		struct sServerData {
			StrId name;
			network2::cUdpServer *svr;
		};

		cUdpServerMap();
		virtual ~cUdpServerMap();

		bool Init(const int startUdpBindPort, const int bindCount
			, const int packetSize = network2::DEFAULT_PACKETSIZE
			, const int packetCount = network2::DEFAULT_PACKETCOUNT
			, const int sleepMillis = network2::DEFAULT_SLEEPMILLIS
			, const int logId = -1
			, const bool isThreadMode = true
		);
		int AddUdpServer(const StrId &name, network2::iProtocolHandler *handler);
		bool RemoveUdpServer(const StrId &name);
		void SetReadyBindPort(const int bindPort);
		sServerData& FindUdpServer(const StrId &name);
		void Clear();


	protected:
		int GetReadyBindPort();
		static int ThreadFunction(cUdpServerMap *udpSvrMap);


	public:
		int m_packetSize;
		int m_packetCount;
		int m_sleepMillis;
		int m_logId; // packet log id, default: -1
		bool m_isThreadMode; // udpserver thread mode? default: true
		vector<std::pair<int, bool>> m_bindPortMap; // port mapping (port, available)
		map<StrId, sServerData> m_svrs;

		// Thread Message
		struct sThreadMsg
		{
			enum class eType { StartServer, RemoveServer };
			eType type;
			StrId name;
			int port;
			network2::iProtocolHandler *handler;
		};
		bool m_isLoop; // thread loop?
		CriticalSection m_csMsg; // sync msg
		CriticalSection m_csSvr; // sync m_svrs
		vector<sThreadMsg> m_sendThreadMsgs; // external -> thread msg
		vector<sThreadMsg> m_recvThreadMsgs;
		std::thread m_thread;

		// spawn udpserver thread
		common::cWQSemaphore m_spawnThread; // run no thread mode
	};

}


// cUdpServerMap Error Handling Protocol
//	- override udpsvrmap::ProtocolHanlder interface
namespace udpsvrmap {
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

