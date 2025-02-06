//
// 2025-01-23, jjuiddong
// udp client with send/recv
//	- 1:1 communication
//
#pragma once


namespace network2
{

	class cUdpClient2 : public cNetworkNode
	{
	public:
		explicit cUdpClient2(
			const StrId &name = "UdpClient2"
			, const int logId = -1
		);
		virtual ~cUdpClient2();

		bool Init(const Str16 &ip
			, const int port
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThreadMode = true );
		bool Process();
		virtual void Close() override;

		// Override
		virtual SOCKET GetSocket(const netid netId) override;
		virtual netid GetNetIdFromSocket(const SOCKET sock) override;
		virtual void GetAllSocket(OUT map<netid, SOCKET> &out) override;
		virtual int Send(const netid rcvId, const cPacket &packet) override;
		virtual int SendImmediate(const netid rcvId, const cPacket &packet) override;
		virtual int SendAll(const cPacket &packet, set<netid> *outErrs = nullptr) override { return 0; }


	protected:
		static int ThreadFunction(cUdpClient2 *client);


	public:
		bool m_isThreadMode; // thread mode?
		int m_maxBuffLen; // recv buffer size
		sockaddr_in m_sockaddr;
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;

		std::thread m_thread;
		int m_sleepMillis;
		char* m_recvBuffer;
	};

}
