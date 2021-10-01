//
// 2019-07-01, jjuiddong 
// mulitple udp server managing class
//
// 2021-09-27
//	- add UdpServer Bind Error, Close Handling Protocol
//		- override udpsvrmap::ProtocolHanlder interface
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

		bool Init(const int startUdpBindPort
			, const int maxServerCount
			, const int packetSize = network2::DEFAULT_PACKETSIZE
			, const int packetCount = network2::DEFAULT_PACKETCOUNT
			, const int sleepMillis = network2::DEFAULT_SLEEPMILLIS
			, const int logId = -1
			, const bool isThreadMode = true
		);
		int AddUdpServer(const StrId &name, network2::iProtocolHandler *handler);
		bool RemoveUdpServer(const StrId &name);
		sServerData& FindUdpServer(const StrId &name);
		void Clear();


	protected:
		void SetReadyPort(const int port);
		void SetPortError(const int port, const int error);
		int GetReadyPort();
		static int ThreadFunction(cUdpServerMap *udpSvrMap);


	public:
		// udpserver port information
		struct sBindPort
		{
			int port;
			bool used;
			int error; // error code, greater than 0
		};

		int m_packetSize;
		int m_packetCount;
		int m_sleepMillis;
		int m_logId; // packet log id, default: -1
		bool m_isThreadMode; // udpserver thread mode? default: true
		vector<sBindPort> m_bindPortMap; // port mapping (port, available)
		map<StrId, sServerData> m_svrs;
		uint m_portCursor;

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
		queue<network2::cUdpServer*> m_freeSvrs; // reuse free udpserver
		//common::cWQSemaphore m_spawnThread; // run no thread mode
	};

}
