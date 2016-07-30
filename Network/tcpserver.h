//
// 2015-11-28, jjuiddong
//
// TCP/IP 프로토콜을 이용해서 통신하는 객체다.
// 최대한 심플하게 만들었다.
//
#pragma once

#include "session.h"
#include "packetqueue.h"

namespace network
{
	class cTCPServer
	{
	public:
		cTCPServer();
		virtual ~cTCPServer();

		bool Init(const int port, const int packetSize=512, const int maxPacketCount=10, const int sleepMillis = 30);
		void Close();
		bool IsConnect() const;
		void MakeFdSet(OUT fd_set &out);
		bool AddSession(const SOCKET remoteSock);
		void RemoveSession(const SOCKET remoteSock);
		bool IsExistSession();


		SOCKET m_svrSocket;
		vector<sSession> m_sessions;
		int m_port;
		bool m_isConnect;
 		int m_maxBuffLen;
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;

		HANDLE m_handle;
		DWORD m_threadId;
		CRITICAL_SECTION m_criticalSection;
		bool m_threadLoop;
		int m_sleepMillis;
		int m_sendBytes; // debug
	};


	inline bool cTCPServer::IsConnect() const { return m_isConnect; }
}
