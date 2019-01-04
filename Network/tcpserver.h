//
// 2015-11-28, jjuiddong
//
// TCP/IP 프로토콜을 이용해서 통신하는 객체다.
// 최대한 심플하게 만들었다.
//
// 2018-12-09, jjuiddong
//	- send all error, close socket
//
// 2019-01-04
//		- update iProtocol
//
#pragma once

#include "session.h"
#include "packetqueue.h"

namespace network
{
	class iSessionListener
	{
	public:
		virtual void RemoveSession(const sSession &session) = 0;
		virtual void AddSession(const sSession &session) = 0;
	};

	class cTCPServer
	{
	public:
		cTCPServer(iProtocol *protocol = new cProtocol());
		virtual ~cTCPServer();

		bool Init(const int port, const int packetSize = 512
			, const int maxPacketCount = 10, const int sleepMillis = 30);
		void SetListener(iSessionListener *listener);
		void Close();
		bool IsConnect() const;
		void MakeFdSet(OUT fd_set &out);
		bool AddSession(const SOCKET remoteSock, const string &ip, const int port);
		void RemoveSession(const SOCKET remoteSock);
		bool IsExistSession();

		static unsigned WINAPI TCPServerThreadFunction(void* arg);


	public:
		SOCKET m_svrSocket;
		vector<sSession> m_sessions;
		iSessionListener *m_listener;
		int m_port;
		bool m_isConnect;
 		int m_maxBuffLen;
		iProtocol *m_protocol;
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;

		std::thread m_thread;
		common::CriticalSection m_criticalSection;
		bool m_threadLoop;
		int m_sleepMillis;
		int m_sendBytes; // debug
	};

}
