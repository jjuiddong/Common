//
// 2019-09-02, jjuiddong
// udp client manager
//	- port managing
//	- thread managing
//	- select multi thread, single thread
//
#pragma once


namespace network2
{

	class cUdpClientMap
	{
	public:
		struct sClientData {
			StrId name;
			network2::cUdpClient *client;
		};

		cUdpClientMap();
		virtual ~cUdpClientMap();

		bool Init(const int startUdpPort, const int portCount
			, const int packetSize = network2::DEFAULT_PACKETSIZE
			, const int packetCount = network2::DEFAULT_MAX_PACKETCOUNT
			, const int sleepMillis = network2::DEFAULT_SLEEPMILLIS
			, const bool isPacketLog = false
			, const bool isThread = false
		);
		int AddUdpClient(const StrId &name, const Str16 &ip, const int udpPort=-1);
		bool RemoveUdpClient(const StrId &name);
		void SetReadyPort(const int readyPort);
		sClientData& FindUdpClient(const StrId &name);
		void Clear();


	protected:
		int GetReadyPort();
		static int ThreadFunction(cUdpClientMap *udpSvrMap);


	public:
		vector<std::pair<int, bool>> m_portMap; // 사용할수있는 UDP Port, (port, available)
		map<StrId, sClientData> m_clients;
		int m_packetSize;
		int m_packetCount;
		int m_sleepMillis;
		bool m_isPacketLog;
		bool m_isThreadMode;

		// Thread
		struct sThreadMsg
		{
			enum TYPE { START_CLIENT, REMOVE_CLIENT };
			TYPE type;
			StrId name;
			Str16 ip;
			int port;
		};
		std::thread m_thread;
		bool m_isThreadLoop;
		CriticalSection m_cs;
		vector<sThreadMsg> m_sendThreadMsgs;
		vector<sThreadMsg> m_recvThreadMsgs;
	};

}
