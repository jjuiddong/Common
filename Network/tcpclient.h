//
// 2015-11-28, jjuiddong
//
// TCP/IP 프로토콜을 이용해서 통신하는 객체다.
// 최대한 심플하게 만들었다.
//
// 2019-01-04
//		- update iProtocol
//
#pragma once

#include "packetqueue.h"


namespace network
{
	class cTCPClient
	{
	public:
		cTCPClient(iProtocol *protocol = new cProtocol());
		virtual ~cTCPClient();

		bool Init(const string &ip, const int port, 
			const int packetSize = 512, const int maxPacketCount = 10, const int sleepMillis = 30);
		void Send(iProtocol *protocol, BYTE *buff, const int len);
		void Close();
		bool IsConnect() const;

		static unsigned WINAPI TCPClientThreadFunction(void* arg);


	public:
		string m_ip;
		int m_port;
		bool m_isConnect;
		SOCKET m_socket; // server socket
		int m_maxBuffLen;
		iProtocol *m_protocol;
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;

		std::thread m_thread;
		bool m_threadLoop;
		int m_sleepMillis;
		int m_recvBytes; // debug
		bool m_isLog; // debug
	};


	inline bool cTCPClient::IsConnect() const { return m_isConnect;  }
}
