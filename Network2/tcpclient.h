//
// 2019-01-08, jjuiddong
// TCP Client
//
#pragma once


namespace network2
{

	class cTcpClient : public cNetworkNode
	{
	public:
		explicit cTcpClient(
			const StrId &name = "TcpClient"
			, const int logId = -1
		);
		virtual ~cTcpClient();

		bool Init(const Str16 &ip
			, const int port
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const int clientSidePort = -1
			, const bool isThreadMode = true
		);
		bool ReConnect();
		bool Process();
		bool RemoveSession();
		void SetSessionListener(iSessionListener *listener);
		virtual void Close() override;

		// Override
		virtual SOCKET GetSocket(const netid netId) override;
		virtual netid GetNetIdFromSocket(const SOCKET sock) override;
		virtual void GetAllSocket(OUT map<netid, SOCKET> &out) override;
		virtual int Send(const netid rcvId, const cPacket &packet) override;
		virtual int SendImmediate(const netid rcvId, const cPacket &packet) override;
		virtual int SendAll(const cPacket &packet, set<netid> *outErrs = nullptr) override { return 0; }


	protected:
		bool ConnectServer();
		static int ThreadFunction(cTcpClient *client);


	public:
		bool m_isThreadMode; // thread mode?
		int m_maxBuffLen; // recv buffer size
		int m_clientSidePort;
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;
		iSessionListener *m_sessionListener;

		std::thread m_thread;
		CriticalSection m_cs;
		int m_sleepMillis;
		char *m_recvBuffer;
	};	

}
