//
// 2019-01-08, jjuiddong
// Network Controller
//
// Server, Client, Dispatcher, Packet Queue, Thread Management
//
#pragma once


namespace network2
{

	class cNetController
	{
	public:
		cNetController();
		virtual ~cNetController();

		bool StartTcpServer(cTcpServer *svr
			, const int bindPort
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThreadMode = true
		);

		bool StartTcpClient(cTcpClient *client
			, const Str16 &ip
			, const int port
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const int clientSidePort = -1	
			, const bool isThread = true
		);

		bool StartUdpServer(cUdpServer *svr
			, const int bindPort
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThreadMode = true
		);

		bool StartUdpClient(cUdpClient *client
			, const Str16 &ip
			, const int port
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThread = true
		);

		bool StartWebServer(cWebServer *svr
			, const int bindPort
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThreadMode = true
			, const bool isSpawnHttpSvr = true
		);

		bool StartWebClient(cWebClient *client
			, const string &url
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThread = true
		);

		int Process(const float deltaSeconds);
		static int Dispatch(cNetworkNode *node);
		
		bool RemoveServer(cNetworkNode *svr);
		bool RemoveClient(cNetworkNode *cli);
		bool IsExistServer(const cNetworkNode *svr);
		bool IsExistClient(const cNetworkNode *cli);
		void Clear();

		
	public:
		vector<cTcpServer*> m_tcpServers; // reference
		vector<cTcpClient*> m_tcpClients; // reference
		vector<cUdpServer*> m_udpServers; // reference
		vector<cUdpClient*> m_udpClients; // reference
		vector<cWebServer*> m_webServers; // reference
		vector<cWebClient*> m_webClients; // reference
	};

}
