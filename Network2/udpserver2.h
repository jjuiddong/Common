//
// 2024-11-23, jjuiddong
// udp server with send/recv
//	- 1:1 communication
//
#pragma once


namespace network2
{

	class cUdpServer2 : public cNetworkNode
	{
	public:
		explicit cUdpServer2(
			const StrId &name = "UdpServer2"
			, const int logId = -1
		);
		virtual ~cUdpServer2();

		bool Init(const int bindPort
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThreadMode = true
		);
		bool ConnectClient(const Str16& ip, const int port);
		bool Process();
		bool ErrorSession(const netid netId);
		virtual void Close() override;

		// Override
		virtual SOCKET GetSocket(const netid netId) override;
		virtual netid GetNetIdFromSocket(const SOCKET sock) override;
		virtual void GetAllSocket(OUT map<netid, SOCKET> &out) override;
		virtual int Send(const netid rcvId, const cPacket &packet) override;
		virtual int SendImmediate(const netid rcvId, const cPacket &packet) override;
		virtual int SendAll(const cPacket &packet, set<netid> *outErrs = nullptr) override;


	protected:
		static unsigned WINAPI ThreadFunction(cUdpServer2*server);


	public:
		int m_maxBuffLen; // recv buffer size
		sockaddr_in m_sockaddr;
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;

		std::thread m_thread;
		int m_sleepMillis;
		char *m_recvBuffer;
	};

}
