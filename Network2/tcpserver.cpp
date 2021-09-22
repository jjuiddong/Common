
#include "stdafx.h"
#include "tcpserver.h"

using namespace network2;
using namespace std;


cTcpServer::cTcpServer(
	iSessionFactory *sessionFactory //= new cSessionFactory()
	, const StrId &name //= "TcpServer"
	, const int logId //= -1
)
	: cNetworkNode(name, logId)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_sessionFactory(sessionFactory)
	, m_sendQueue(this, logId)
	, m_recvQueue(this, logId)
	, m_sessionListener(nullptr)
	, m_tempRecvBuffer(nullptr)
	, m_lastAcceptTime(0)
	, m_isThreadMode(true)
	, m_isUpdateSocket(false)
{
}

cTcpServer::~cTcpServer()
{
	Close();

	SAFE_DELETE(m_sessionFactory);
}


bool cTcpServer::Init(const int bindPort
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_MAX_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //= true;
)
{
	Close();

	m_ip = "localhost";
	m_port = bindPort;
	m_sleepMillis = sleepMillis;
	m_maxBuffLen = packetSize;
	m_isThreadMode = isThreadMode;
	m_timer.Create();

	if (network2::LaunchTCPServer(bindPort, m_socket))
	{
		m_state = eState::Connect;
		dbg::Logc(1, "Bind TCP Server port = %d\n", bindPort);

		if (!m_recvQueue.Init(packetSize, maxPacketCount))
		{
			goto $error;
		}

		if (!m_sendQueue.Init(packetSize, maxPacketCount))
		{
			goto $error;
		}
	}
	else
	{
		dbg::Logc(2, "Error!! Bind TCP Server port=%d\n", bindPort);
		goto $error;
	}

	if (isThreadMode)
	{
		m_thread = std::thread(cTcpServer::ThreadFunction, this);
	}

	return true;


$error:
	Close();
	return false;
}


// Network Packet Recv/Send
// for single thread
bool cTcpServer::Process()
{
	if (!m_tempRecvBuffer)
		m_tempRecvBuffer = new char[m_maxBuffLen];

	AcceptProcess();

	ReceiveProcces();

	// Send Packet
	{
		map<netid, SOCKET> socks;
		GetAllSocket(socks);

		set<netid> errSocks;
		m_sendQueue.SendAll(socks, &errSocks);
		for (auto sock : errSocks)
		{
			const netid netId = GetNetIdFromSocket(sock);
			if (netId != INVALID_NETID)
				m_recvQueue.Push(netId, DisconnectPacket(this, netId));
		}
	}

	return true;
}


// Accept Client From TCP/IP Socket
bool cTcpServer::AcceptProcess()
{
	// Accept는 가끔씩 처리한다.
	const double curT = m_timer.GetMilliSeconds();
	if (curT - m_lastAcceptTime < 30.f)
		return true;

	m_lastAcceptTime = curT;

	// Accept Client
	const timeval t = { 0, 0 };
	fd_set acceptSockets;
	FD_ZERO(&acceptSockets);
	FD_SET(m_socket, &acceptSockets);
	const int ret = select(acceptSockets.fd_count, &acceptSockets, NULL, NULL, &t);
	if (ret == 0)
		return true; // nothing~
	if (ret == SOCKET_ERROR)
		return true; // error occur

	int sockLen = sizeof(sockaddr_in);
	sockaddr_in addr;
	SOCKET remoteSocket = accept(acceptSockets.fd_array[0], (sockaddr*)&addr, &sockLen);
	if (remoteSocket == INVALID_SOCKET)
	{
		// error occurred!
		return true;
	}

	getpeername(remoteSocket, (struct sockaddr*)&addr, &sockLen);
	char *clientAddr = inet_ntoa(addr.sin_addr);
	const int port = htons(addr.sin_port);
	if (!m_recvQueue.Push(SERVER_NETID, AcceptPacket(this, remoteSocket, clientAddr, port)))
	{
		// Error Occur!
		// packet queue error
		// remove remoteSocket
		closesocket(remoteSocket);
	}

	return true;
}


// Packet Receive From TCP/IP Socket
bool cTcpServer::ReceiveProcces()
{
	fd_set readSockets;
	if (m_isUpdateSocket)
	{
		common::AutoCSLock cs(m_cs);
		readSockets = m_sockets;
		m_isUpdateSocket = false;
	}
	else
	{
		readSockets = m_sockets;
	}
	//MakeFdSet(readSockets);
	if (0 == readSockets.fd_count)
		return true;

	const timeval t = { 0, 1 };
	const fd_set sockets = readSockets;

	const int ret = select(readSockets.fd_count, &readSockets, NULL, NULL, &t);
	if (ret == 0)
		return true; // nothing
	if (ret == SOCKET_ERROR)
		return false; // error occur, nothing

	for (u_int i = 0; i < sockets.fd_count; ++i)
	{
		if (!FD_ISSET(sockets.fd_array[i], &readSockets))
			continue;

		const int result = recv(sockets.fd_array[i], m_tempRecvBuffer, m_maxBuffLen, 0);
		const netid netId = GetNetIdFromSocket(sockets.fd_array[i]);
		if (netId == INVALID_NETID)
			continue; // not found netid

		if (result == SOCKET_ERROR || result == 0) 
		{
			// when receive packet size == 0, disconnect state
			if (!m_recvQueue.Push(netId, DisconnectPacket(this, netId)))
				RemoveSession(netId); // exception process
		}
		else
		{
			m_recvQueue.Push(netId, (BYTE*)m_tempRecvBuffer, result);
		}
	}

	return true;
}


// update session listener
void cTcpServer::SetSessionListener(iSessionListener *listener)
{
	m_sessionListener = listener;
}


void cTcpServer::Close()
{
	m_state = eState::Disconnect;
	if (m_thread.joinable())
		m_thread.join();

	{
		AutoCSLock cs(m_cs);
		for (auto &session : m_sessions.m_seq)
			SAFE_DELETE(session);
		m_sessions.clear();
		m_sessions2.clear();
	}

	SAFE_DELETEA(m_tempRecvBuffer);
	cNetworkNode::Close();
}


void cTcpServer::MakeFdSet(OUT fd_set &out)
{
	AutoCSLock cs(m_cs);

	FD_ZERO(&out);
	for (auto &session : m_sessions.m_seq)
		FD_SET(session->m_socket, (fd_set*)&out);
}


// return socket correspond netid
SOCKET cTcpServer::GetSocket(const netid netId)
{
	if (netId == m_id)
		return m_socket;

	common::AutoCSLock cs(m_cs);

	auto it = m_sessions.find(netId);
	if (it == m_sessions.end())
		return INVALID_SOCKET;
	return it->second->m_socket;
}


// return netid correspond socket
netid cTcpServer::GetNetIdFromSocket(const SOCKET sock)
{
	if (sock == m_socket)
		return m_id;

	{
		common::AutoCSLock cs(m_cs);
		auto it = m_sessions2.find(sock);
		if (it == m_sessions2.end())
			return INVALID_NETID;
		return it->second->m_id;
	}
	return INVALID_NETID;
}


void cTcpServer::GetAllSocket(OUT map<netid, SOCKET> &out)
{
	common::AutoCSLock cs(m_cs);
	for (auto &sock : m_sessions2.m_seq)
		out.insert({ sock->m_id, sock->m_socket });
}


// send packet 
int cTcpServer::Send(const netid rcvId, const cPacket &packet)
{
	m_sendQueue.Push(rcvId, packet);
	return 1;
}


int cTcpServer::SendImmediate(const netid rcvId, const cPacket &packet)
{
	assert(0); // not implements
	return 1;
}


// send packet to all client
int cTcpServer::SendAll(const cPacket &packet)
{
	return 1; // not implements
}


// Add Session
bool cTcpServer::AddSession(const SOCKET sock, const Str16 &ip, const int port)
{
	RETV(!m_sessionFactory, false);

	if (FindSessionBySocket(sock))
		return false; // Error!! Already Exist

	common::AutoCSLock cs(m_cs);
	cSession *session = m_sessionFactory->New();
	session->m_id = common::GenerateId();
	session->m_socket = sock;
	session->m_ip = ip;
	session->m_port = port;
	session->m_state = eState::Connect;
	m_sessions.insert({ session->m_id, session });
	m_sessions2.insert({ sock, session });
	m_isUpdateSocket = true; // update socket list
	FD_SET(sock, &m_sockets);

	if (m_logId >= 0)
		network2::LogSession(m_logId, *session);

	if (m_sessionListener)
		m_sessionListener->AddSession(*session);

	return true;
}


// Remove Session
bool cTcpServer::RemoveSession(const netid netId)
{
	cSession *session = NULL;
	{
		common::AutoCSLock cs(m_cs);

		auto it = m_sessions.find(netId);
		if (m_sessions.end() == it)
			return false; // not found session
		session = it->second;
	}

	bool isRemove = true;
	if (m_sessionListener)
		isRemove = m_sessionListener->RemoveSession(*session);

	{
		common::AutoCSLock cs(m_cs);

		const SOCKET sock = session->m_socket;
		if (isRemove)
		{
			session->Close();
			SAFE_DELETE(session);
		}
		m_sessions.remove(netId);
		m_sessions2.remove(sock);
		m_recvQueue.Remove(netId);
		m_isUpdateSocket = true;
		FD_CLR(sock, &m_sockets);
	}

	return true;
}


cSession* cTcpServer::FindSessionBySocket(const SOCKET sock)
{
	common::AutoCSLock cs(m_cs);

	auto it = m_sessions2.find(sock);
	if (m_sessions2.end() == it)
		return NULL; // not found session
	return it->second;
}


cSession* cTcpServer::FindSessionByNetId(const netid netId)
{
	common::AutoCSLock cs(m_cs);

	auto it = m_sessions.find(netId);
	if (m_sessions.end() == it)
		return NULL; // not found session
	return it->second;
}


cSession* cTcpServer::FindSessionByName(const StrId &name)
{
	common::AutoCSLock cs(m_cs);

	for (auto &session : m_sessions.m_seq)
		if (session->m_name == name)
			return session;
	return NULL; // not found session
}


// check exist session correspond netid
bool cTcpServer::IsExist(const netid netId)
{
	common::AutoCSLock cs(m_cs);

	auto it = m_sessions.find(netId);
	return m_sessions.end() != it;
}



// TCP Server Thread Function
unsigned WINAPI cTcpServer::ThreadFunction(cTcpServer* server)
{
	while (eState::Connect == server->m_state)
	{
		server->Process();

		const int sleepTime = server->m_sessions.empty() ? 100 : server->m_sleepMillis;
		if (sleepTime > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}

	return 0;
}
