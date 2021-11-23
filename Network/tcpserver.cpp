
#include "stdafx.h"
#include "tcpserver.h"


using namespace std;
using namespace network;


cTCPServer::cTCPServer(
	iProtocol *protocol //= new cProtocol()
)
	: m_isConnect(false)
	, m_listener(NULL)
	, m_threadLoop(true)
	, m_maxBuffLen(BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_sendBytes(0)
	, m_protocol(protocol)
	, m_sendQueue(protocol)
	, m_recvQueue(protocol)
{
}

cTCPServer::~cTCPServer()
{
	Close();
	SAFE_DELETE(m_protocol);
}


bool cTCPServer::Init(const int port
	, const int packetSize //=512
	, const int maxPacketCount //=10
	, const int sleepMillis //=30
)
{
	Close();

	m_port = port;
	m_sleepMillis = sleepMillis;
	m_maxBuffLen = packetSize;

	if (network::LaunchTCPServer(port, m_svrSocket))
	{
		cout << "Bind TCP Server " << "port = " << port << endl;

		if (!m_recvQueue.Init(packetSize, maxPacketCount))
		{
			Close();
			return false;
		}

		if (!m_sendQueue.Init(packetSize, maxPacketCount))
		{
			Close();
			return false;
		}

		m_threadLoop = false;
		if (m_thread.joinable())
			m_thread.join();

		m_isConnect = true;
		m_threadLoop = true;
		m_thread = std::thread(cTCPServer::TCPServerThreadFunction, this);
	}
	else
	{
		cout << "Error!! Bind TCP Server port=" << port << endl;
		return false;
	}

	return true;
}


bool cTCPServer::IsConnect() const 
{ 
	return m_isConnect; 
}


void cTCPServer::Close()
{
	m_isConnect = false;
	m_threadLoop = false;
	if (m_thread.joinable())
		m_thread.join();

	closesocket(m_svrSocket);
	m_svrSocket = INVALID_SOCKET;
}


void cTCPServer::MakeFdSet(OUT fd_set &out)
{
	FD_ZERO(&out);
	for each (auto &session in m_sessions)
	{
		FD_SET(session.socket, (fd_set*)&out);
	}
}


bool cTCPServer::AddSession(const SOCKET remoteSock, const string &ip, const int port)
{
	AutoCSLock cs(m_criticalSection);

	for each (auto &session in m_sessions)
	{
		if (remoteSock == session.socket)
			return false; // 이미 있다면 종료.
	}

	sSession session;
	session.state = SESSION_STATE::LOGIN_WAIT;
	session.socket = remoteSock;
	strcpy_s(session.ip, ip.c_str());
	session.port = port;
	m_sessions.push_back(session);

	if (m_listener)
		m_listener->AddSession(session);

	return true;
}


void cTCPServer::RemoveSession(const SOCKET remoteSock)
{
	AutoCSLock cs(m_criticalSection);

	for (u_int i = 0; i < m_sessions.size(); ++i)
	{
		if (remoteSock == m_sessions[i].socket)
		{
			if (m_listener)
				m_listener->RemoveSession(m_sessions[i]);

			closesocket(m_sessions[i].socket);
			common::removevector2(m_sessions, i);
			break;
		}
	}
}


void cTCPServer::SetListener(iSessionListener *listener)
{
	m_listener = listener;
}


bool cTCPServer::IsExistSession()
{
	AutoCSLock cs(m_criticalSection);
	return m_isConnect && !m_sessions.empty();
}


unsigned WINAPI cTCPServer::TCPServerThreadFunction(void* arg)
{
	cTCPServer *server = (cTCPServer*)arg;

	char *buff = new char[server->m_maxBuffLen];
 	const int maxBuffLen = server->m_maxBuffLen;

	common::cTimer m_timer;
	m_timer.Create();

	double lastAcceptTime = m_timer.GetMilliSeconds();

	while (server->m_threadLoop)
	{
		const timeval t = { 0, 1};

		// Accept는 가끔씩 처리한다.
		const double curT = m_timer.GetMilliSeconds();
		if (curT - lastAcceptTime > 300.f)
		{
			lastAcceptTime = curT;

			//-----------------------------------------------------------------------------------
			// Accept Client
			fd_set acceptSockets;
			FD_ZERO(&acceptSockets);
			FD_SET(server->m_svrSocket, &acceptSockets);
			const int ret1 = select(acceptSockets.fd_count, &acceptSockets, NULL, NULL, &t);
			if (ret1 != 0 && ret1 != SOCKET_ERROR)
			{
				// accept(요청을 받은 소켓, 선택 클라이언트 주소)
				int sockLen = sizeof(sockaddr_in);
				sockaddr_in addr;
				SOCKET remoteSocket = accept(acceptSockets.fd_array[0], (sockaddr*)&addr, &sockLen);
				if (remoteSocket == INVALID_SOCKET)
				{
	 				//Client를 Accept하는 도중에 에러가 발생함
					continue;
				}

				getpeername(remoteSocket, (struct sockaddr*)&addr, &sockLen);
				char *clientAddr = inet_ntoa(addr.sin_addr);
				const int port = htons(addr.sin_port);
				server->AddSession(remoteSocket, clientAddr, port);
			}
			//-----------------------------------------------------------------------------------
		}


		//-----------------------------------------------------------------------------------
		// Receive Packet
		fd_set readSockets;
		server->MakeFdSet(readSockets);
		const fd_set sockets = readSockets;

		const int ret = select(readSockets.fd_count, &readSockets, NULL, NULL, &t);
		if (ret != 0 && ret != SOCKET_ERROR)
		{
			for (u_int i = 0; i < sockets.fd_count; ++i)
			{
				if (!FD_ISSET(sockets.fd_array[i], &readSockets)) 
					continue;

				const int result = recv(sockets.fd_array[i], buff, maxBuffLen, 0);
				if (result == SOCKET_ERROR || result == 0) // 받은 패킷사이즈가 0이면 서버와 접속이 끊겼다는 의미다.
				{
					server->RemoveSession(sockets.fd_array[i]);
				}
				else
				{
					server->m_recvQueue.PushFromNetwork(sockets.fd_array[i], (BYTE*)buff, result);
				}
			}
		}
		//-----------------------------------------------------------------------------------


		//-----------------------------------------------------------------------------------
		// Send Packet
		{
			vector<SOCKET> errSocks;
			server->m_sendQueue.SendAll(&errSocks);

			for (auto sock : errSocks)
				server->RemoveSession(sock);
		}
		//-----------------------------------------------------------------------------------

		const int sleepTime = server->m_sessions.empty() ? 100 : server->m_sleepMillis;
		if (sleepTime > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}

	delete[] buff;
	return 0;
}
