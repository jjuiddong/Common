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
			, const int maxPacketCount = DEFAULT_MAX_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const int clientSidePort = -1
			, const bool isThreadMode = true
		);
		bool ReConnect();
		bool Process();
		virtual void Close() override;

		// Override
		virtual SOCKET GetSocket(const netid netId) override;
		virtual netid GetNetIdFromSocket(const SOCKET sock) override;
		virtual void GetAllSocket(OUT map<netid, SOCKET> &out) override;
		virtual int Send(const netid rcvId, const cPacket &packet) override;
		virtual int SendAll(const cPacket &packet) override { return 0;  }


	protected:
		bool ConnectServer();
		static int ThreadFunction(cTcpClient *client);


	public:
		bool m_isThreadMode;
		int m_clientSidePort;
		int m_maxBuffLen;
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;

		std::thread m_thread;
		CriticalSection m_cs;
		int m_sleepMillis;
		char *m_recvBuffer;
	};	

}
