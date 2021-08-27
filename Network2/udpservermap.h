//
// 2019-07-01, jjuiddong 
// mulitple udp server managing class
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
			, const int packetCount = network2::DEFAULT_MAX_PACKETCOUNT
			, const int sleepMillis = network2::DEFAULT_SLEEPMILLIS
			, const int logId = -1
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
		vector<std::pair<int, bool>> m_bindPortMap; // 사용할수있는 UDP Bind Port, (port, available)
		map<StrId, sServerData> m_svrs;
		int m_packetSize;
		int m_packetCount;
		int m_sleepMillis;
		int m_logId; // packet log id, default: -1

		// Thread
		struct sThreadMsg
		{
			enum TYPE { START_SERVER, REMOVE_SERVER };
			TYPE type;
			StrId name;
			int port;
			network2::iProtocolHandler *handler;
		};
		std::thread m_thread;
		bool m_isThreadLoop;
		CriticalSection m_cs;
		vector<sThreadMsg> m_sendThreadMsgs;
		vector<sThreadMsg> m_recvThreadMsgs;
	};

}
