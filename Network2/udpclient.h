//
// 2019-01-11, jjuiddong
// udp client
//
#pragma once


namespace network2
{

	class cUdpClient : public cNetworkNode
	{
	public:
		cUdpClient(const bool isPacketLog = false
			, const StrId &name = "UdpClient"
		);
		virtual ~cUdpClient();

		bool Init(const Str16 &ip
			, const int port
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_MAX_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThreadMode = true );
		bool Process();
		virtual void Close() override;

		// Override
		virtual SOCKET GetSocket(const netid netId) override;
		virtual netid GetNetIdFromSocket(const SOCKET sock) override;
		virtual void GetAllSocket(OUT map<netid, SOCKET> &out) override;
		virtual int Send(const netid rcvId, const cPacket &packet) override;
		virtual int SendAll(const cPacket &packet) override { return 0; }


	protected:
		static int ThreadFunction(cUdpClient *client);


	public:
		bool m_isThreadMode;
		int m_maxBuffLen;
		sockaddr_in m_sockaddr;
		cPacketQueue m_sendQueue;

		std::thread m_thread;
		int m_sleepMillis;
	};

}
