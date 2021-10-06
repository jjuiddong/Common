//
// 2021-10-06, jjuiddong 
// mulitple udp server managing class
//	- udp socket polling system using select()
//	- error, close event trigger with udpsvrmap::ProtocolHanlder
//
#pragma once


namespace network2
{

	class cUdpServerMap2
	{
	public:
		struct sServerData {
			StrId name;
			network2::cUdpServer *svr;
		};

		cUdpServerMap2();
		virtual ~cUdpServerMap2();

		bool Init(const int startUdpBindPort
			, const int maxServerCount
			, const int packetSize = network2::DEFAULT_PACKETSIZE
			, const int packetCount = network2::DEFAULT_PACKETCOUNT
			, const int sleepMillis = network2::DEFAULT_SLEEPMILLIS
			, const int logId = -1
		);
		int AddUdpServer(const StrId &name, network2::iProtocolHandler *handler);
		bool RemoveUdpServer(const StrId &name);
		sServerData& FindUdpServer(const StrId &name);
		void Clear();


	protected:
		void SetReadyPort(const int port);
		void SetPortError(const int port, const int error);
		int GetReadyPort();
		bool MessageProcess();
		bool ReceiveProcess();
		static int ThreadFunction(cUdpServerMap2 *udpSvrMap);


	public:
		// udpserver port information
		struct sBindPort
		{
			int port;
			bool used; // use port?
			int error; // error code, greater than 0
		};

		int m_packetSize; // packet buffer size
		int m_packetCount; // maximum packet count
		int m_sleepMillis; // sleep time, milliseconds unit
		int m_logId; // packet log id, default: -1
		vector<sBindPort> m_bindPortMap; // port mapping (port, available)
		uint m_portCursor;
		map<StrId, sServerData> m_svrs;
		map<SOCKET, cUdpServer*> m_svrs2; // reference from m_svrs, key: udpserver socket
		queue<network2::cUdpServer*> m_freeSvrs; // reuse free udpserver
		fd_set m_readSockets; // read udp socket set

		// Thread Message
		struct sThreadMsg
		{
			enum eType { StartServer, RemoveServer };
			eType type;
			StrId name;
			int port;
			network2::iProtocolHandler *handler;
		};

		bool m_isLoop; // thread loop?
		CriticalSection m_csMsg; // sync message
		vector<sThreadMsg> m_sendThreadMsgs; // external -> thread msg
		char *m_tempRecvBuffer;
		std::thread m_thread;
	};

}
