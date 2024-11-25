#include "stdafx.h"
#include "udpserver2.h"

using namespace network2;
using namespace std;


cUdpServer2::cUdpServer2(
	const StrId &name //= "UdpServer2"
	, const int logId //= -1
)
	: cNetworkNode(name, logId)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_sendQueue(this, logId)
	, m_recvQueue(this, logId)
	, m_recvBuffer(nullptr)
{
	ZeroMemory(&m_sockaddr, sizeof(m_sockaddr));
}

cUdpServer2::~cUdpServer2()
{
	Close();
	SAFE_DELETEA(m_recvBuffer);
}


// initialize
bool cUdpServer2::Init(const int bindPort
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //= true
)
{
	Close();

	m_port = bindPort;
	m_sleepMillis = sleepMillis;
	m_maxBuffLen = packetSize;

	if (!m_sendQueue.Init(packetSize, maxPacketCount))
		goto $error;

	if (!m_recvQueue.Init(packetSize, maxPacketCount))
		goto $error;

	if (network2::LaunchUDPServer(bindPort, m_socket))
	{
		//dbg::Logc(1, "Bind UDP Server port = %d\n", bindPort);
		m_state = eState::Connect;
	}
	else
	{
		dbg::Logc(2, "Error!! Bind UDP Server port = %d\n", bindPort);
		goto $error;
	}

	if (isThreadMode)
		m_thread = std::thread(cUdpServer2::ThreadFunction, this);
	return true;


$error:
	Close();
	return false;
}


bool cUdpServer2::Process()
{
	if (eState::Connect != m_state)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepMillis));
		return false;
	}

	if (m_sockaddr.sin_port > 0)
		m_sendQueue.SendAll(m_sockaddr);

	if (!m_recvBuffer)
		m_recvBuffer = new char[m_maxBuffLen];

	const timeval t = { 0, m_sleepMillis * 1000 };
	fd_set readSockets;
	FD_ZERO(&readSockets);
	FD_SET(m_socket, &readSockets);

	const int ret = select(readSockets.fd_count, &readSockets, NULL, NULL, &t);
	if (0 == ret)
		return true;
	if (SOCKET_ERROR == ret)
		return false;

	socklen_t fromSize = sizeof(SOCKADDR);
	const int result = recvfrom(readSockets.fd_array[0], (char*)m_recvBuffer, m_maxBuffLen
		, 0, (sockaddr*)&m_sockaddr, &fromSize);
	if (SOCKET_ERROR == result || 0 == result)
	{
		// disconnect state
	}
	else
	{
		const netid netId = GetNetIdFromSocket(readSockets.fd_array[0]);
		if (INVALID_NETID != netId)
			m_recvQueue.Push(netId, (BYTE*)m_recvBuffer, result);
	}

	return true;
}


// error session, calling from UdpServerDispatcher
// udp server bind error
bool cUdpServer2::ErrorSession(const netid netId)
{
	return true;
}


void cUdpServer2::Close()
{
	m_state = eState::Disconnect;
	if (m_thread.joinable())
		m_thread.join();
	__super::Close();
}


SOCKET cUdpServer2::GetSocket(const netid netId)
{
	if ((m_id != netId) && (network2::SERVER_NETID != netId))
	{
		assert(0);
	}
	return m_socket;
}


netid cUdpServer2::GetNetIdFromSocket(const SOCKET sock)
{
	if (m_socket != sock)
	{
		assert(0);
	}
	return m_id;
}


void cUdpServer2::GetAllSocket(OUT map<netid, SOCKET> &out)
{
	out.insert({ m_id, m_socket });
	out.insert({ network2::CLIENT_NETID, m_socket });
}


// send packet 
int cUdpServer2::Send(const netid rcvId, const cPacket &packet)
{
	if (m_sockaddr.sin_port > 0)
		m_sendQueue.Push(rcvId, packet);
	return 1;
}


int cUdpServer2::SendImmediate(const netid rcvId, const cPacket &packet)
{
	assert(0); // not implements
	return 1;
}


// send packet to all client
int cUdpServer2::SendAll(const cPacket &packet, set<netid> *outErrs //= nullptr
)
{
	// udp server only receive packet
	assert(0);
	return 1;
}


// UDP Server Thread Function
unsigned WINAPI cUdpServer2::ThreadFunction(cUdpServer2* udp)
{
	while (eState::Connect == udp->m_state)
	{
		udp->Process();
		//if (udp->m_sleepMillis)
		//	std::this_thread::sleep_for(std::chrono::milliseconds(udp->m_sleepMillis));
	}
	return 0;
}
