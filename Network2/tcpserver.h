//
// 2019-01-08, jjuiddong
// tcp server
//
#pragma once


namespace network2
{

	class cTcpServer : public cNetworkNode
	{
	public:
		cTcpServer(iPacketHeader *packetHeader = new cPacketHeader()
			, iSessionFactory *sessionFactory = new cSessionFactory()
			, const bool isPacketLog = false
			, const StrId &name = "TcpServer"
		);
		virtual ~cTcpServer();

		bool Init(const int bindPort
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_MAX_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThreadMode = true
		);
		bool Process();
		void SetSessionListener(iSessionListener *listener);
		bool AddSession(const SOCKET sock, const Str16 &ip, const int port);
		bool RemoveSession(const netid netId);
		bool IsExist(const netid netId);
		cSession* FindSessionBySocket(const SOCKET sock);
		cSession* FindSessionByNetId(const netid netId);
		cSession* FindSessionByName(const StrId &name);
		void MakeFdSet(OUT fd_set &out);
		virtual void Close() override;

		// Override
		virtual SOCKET GetSocket(const netid netId) override;
		virtual netid GetNetIdFromSocket(const SOCKET sock) override;
		virtual void GetAllSocket(OUT map<netid, SOCKET> &out) override;
		virtual int Send(const netid rcvId, const cPacket &packet) override;
		virtual int SendAll(const cPacket &packet) override;


	protected:
		static unsigned WINAPI ThreadFunction(cTcpServer *server);
		bool AcceptProcess();
		bool ReceiveProcces();


	public:
		bool m_isThreadMode;
		int m_maxBuffLen;
		common::VectorMap<netid, cSession*> m_sessions;
		common::VectorMap<SOCKET, cSession*> m_sockets; // reference
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;
		iSessionListener *m_sessionListener;
		iSessionFactory *m_sessionFactory;

		std::thread m_thread;
		common::CriticalSection m_cs;
		int m_sleepMillis;
		double m_lastAcceptTime;
		char *m_tempRecvBuffer;
		common::cTimer m_timer;
	};

}
