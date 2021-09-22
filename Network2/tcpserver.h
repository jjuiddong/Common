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
		explicit cTcpServer(
			iSessionFactory *sessionFactory = new cSessionFactory()
			, const StrId &name = "TcpServer"
			, const int logId = -1
		);
		virtual ~cTcpServer();

		bool Init(const int bindPort
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_MAX_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThreadMode = true
		);
		bool Process();
		bool AddSession(const SOCKET sock, const Str16 &ip, const int port);
		bool RemoveSession(const netid netId);
		bool IsExist(const netid netId);
		cSession* FindSessionBySocket(const SOCKET sock);
		cSession* FindSessionByNetId(const netid netId);
		cSession* FindSessionByName(const StrId &name);
		void SetSessionListener(iSessionListener *listener);
		void MakeFdSet(OUT fd_set &out);
		virtual void Close() override;

		// Override
		virtual SOCKET GetSocket(const netid netId) override;
		virtual netid GetNetIdFromSocket(const SOCKET sock) override;
		virtual void GetAllSocket(OUT map<netid, SOCKET> &out) override;
		virtual int Send(const netid rcvId, const cPacket &packet) override;
		virtual int SendImmediate(const netid rcvId, const cPacket &packet) override;
		virtual int SendAll(const cPacket &packet) override;


	protected:
		static unsigned WINAPI ThreadFunction(cTcpServer *server);
		bool AcceptProcess();
		bool ReceiveProcces();


	public:
		bool m_isThreadMode;
		int m_maxBuffLen;
		common::VectorMap<netid, cSession*> m_sessions;
		common::VectorMap<SOCKET, cSession*> m_sessions2; // reference
		fd_set m_sockets;
		std::atomic<bool> m_isUpdateSocket; // sync m_sockets
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;
		iSessionFactory *m_sessionFactory;
		iSessionListener *m_sessionListener;

		std::thread m_thread;
		CriticalSection m_cs;
		int m_sleepMillis;
		double m_lastAcceptTime;
		char *m_tempRecvBuffer;
		common::cTimer m_timer;
	};

}
