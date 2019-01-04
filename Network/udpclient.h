//
// v.1
// udp client, send, receive 기능
//
// v.2
// close(), waitforsingleobject
// isConnect()
// thread restart bug fix
//
// 2016-08-30
//		- packetqueue 추가
//
// 2019-01-04
//		- update iProtocol
//
#pragma once


namespace network
{

	class cUDPClient
	{
	public:
		cUDPClient(iProtocol *protocol = new cProtocol());
		virtual ~cUDPClient();

		bool Init(const string &ip, const int port, const int sleepMillis=30);
		void SendData(iProtocol *protocol, const BYTE *buff, const int buffLen);
		void SetMaxBufferLength(const int length);
		void Close();
		bool IsConnect() const;

		static unsigned WINAPI UDPClientThreadFunction(void* arg);


	public:
		string m_ip;
		int m_port;

		bool m_isConnect;
		SOCKET m_socket;
		sockaddr_in m_sockaddr;
		int m_maxBuffLen;
		iProtocol *m_protocol;
		cPacketQueue m_sndQueue;

		std::thread m_thread;
		bool m_threadLoop;
		int m_sleepMillis; // Sleep(m_sleepMillis)
	};


	inline bool cUDPClient::IsConnect() const { return m_isConnect; }

}
