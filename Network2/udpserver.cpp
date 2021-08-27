#include "stdafx.h"
#include "udpserver.h"

using namespace network2;
using namespace std;


cUdpServer::cUdpServer(const StrId &name //= "UdpServer"
	, const int logId //= -1
)
	: cNetworkNode(name, logId)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_recvQueue(this, logId)
	, m_tempRecvBuffer(NULL)
{
}

cUdpServer::~cUdpServer()
{
	Close();
}


bool cUdpServer::Init(const int bindPort
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_MAX_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //= true
)
{
	Close();

	m_port = bindPort;
	m_sleepMillis = sleepMillis;
	m_maxBuffLen = packetSize;

	if (network2::LaunchUDPServer(bindPort, m_socket))
	{
		dbg::Logc(1, "Bind UDP Server port = %d\n", bindPort);

		m_state = CONNECT;
		if (!m_recvQueue.Init(packetSize, maxPacketCount))
		{
			goto $error;
		}
	}
	else
	{
		dbg::Logc(2, "Error!! Bind UDP Server port = %d\n", bindPort);
		goto $error;
	}

	if (isThreadMode)
	{
		m_thread = std::thread(cUdpServer::ThreadFunction, this);
	}

	return true;


$error:
	Close();
	return false;
}


bool cUdpServer::Process()
{
	if (!m_tempRecvBuffer)
		m_tempRecvBuffer = new char[m_maxBuffLen];

	const timeval t = { 0, 1 };
	fd_set readSockets;
	FD_ZERO(&readSockets);
	FD_SET(m_socket, &readSockets);

	const int ret = select(readSockets.fd_count, &readSockets, NULL, NULL, &t);
	if (ret == 0)
		return true;
	if (ret == SOCKET_ERROR)
		return false;

	const int result = recv(readSockets.fd_array[0], (char*)m_tempRecvBuffer, m_maxBuffLen, 0);
	if (result == SOCKET_ERROR || result == 0) // 받은 패킷사이즈가 0이면 서버와 접속이 끊겼다는 의미다.
	{
		// 에러가 발생하더라도, 수신 대기상태로 계속 둔다.
	}
	else
	{
		const netid netId = GetNetIdFromSocket(readSockets.fd_array[0]);
		if (netId != INVALID_NETID)
			m_recvQueue.Push(netId, (BYTE*)m_tempRecvBuffer, result);
	}

	return true;
}


void cUdpServer::Close()
{
	m_state = DISCONNECT;
	if (m_thread.joinable())
		m_thread.join();

	SAFE_DELETEA(m_tempRecvBuffer);
	cNetworkNode::Close();
}


SOCKET cUdpServer::GetSocket(const netid netId)
{
	if ((m_id != netId) && (network2::SERVER_NETID != netId))
	{
		assert(0);
	}
	return m_socket;
}


netid cUdpServer::GetNetIdFromSocket(const SOCKET sock)
{
	if (m_socket != sock)
	{
		assert(0);
	}
	return m_id;
}


void cUdpServer::GetAllSocket(OUT map<netid, SOCKET> &out)
{
	out.insert({ m_id, m_socket });
	out.insert({ network2::CLIENT_NETID, m_socket });
}


// send packet 
int cUdpServer::Send(const netid rcvId, const cPacket &packet)
{
	// UDP Server는 패킷을 받을 수만 있다.
	assert(0);
	return 1;
}


// send packet to all client
int cUdpServer::SendAll(const cPacket &packet)
{
	// UDP Server는 패킷을 받을 수만 있다.
	assert(0);
	return 1;
}


// UDP Server Thread Function
unsigned WINAPI cUdpServer::ThreadFunction(cUdpServer* udp)
{
	while (CONNECT == udp->m_state)
	{
		udp->Process();

		if (udp->m_sleepMillis)
			std::this_thread::sleep_for(std::chrono::milliseconds(udp->m_sleepMillis));
	}

	return 0;
}
