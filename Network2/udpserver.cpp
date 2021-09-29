#include "stdafx.h"
#include "udpserver.h"

using namespace network2;
using namespace std;


cUdpServer::cUdpServer(
	const StrId &name //= "UdpServer"
	, const int logId //= -1
)
	: cNetworkNode(name, logId)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_recvQueue(this, logId)
	, m_recvBuffer(nullptr)
{
}

cUdpServer::~cUdpServer()
{
	Close();
}


bool cUdpServer::Init(const int bindPort
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

	m_recvQueue.Init(packetSize, maxPacketCount);

	if (network2::LaunchUDPServer(bindPort, m_socket))
	{
		dbg::Logc(1, "Bind UDP Server port = %d\n", bindPort);
		m_state = eState::Connect;
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
	RETV(eState::Connect != m_state, false);

	if (!m_recvBuffer)
		m_recvBuffer = new char[m_maxBuffLen];

	const timeval t = { 0, 1 };
	fd_set readSockets;
	FD_ZERO(&readSockets);
	FD_SET(m_socket, &readSockets);

	const int ret = select(readSockets.fd_count, &readSockets, NULL, NULL, &t);
	if (ret == 0)
		return true;
	if (ret == SOCKET_ERROR)
		return false;

	const int result = recv(readSockets.fd_array[0], (char*)m_recvBuffer, m_maxBuffLen, 0);
	if (result == SOCKET_ERROR || result == 0)
	{
		// disconcet state
	}
	else
	{
		const netid netId = GetNetIdFromSocket(readSockets.fd_array[0]);
		if (netId != INVALID_NETID)
			m_recvQueue.Push(netId, (BYTE*)m_recvBuffer, result);
	}

	return true;
}


// error session, calling from UdpServerDispatcher
// udp server bind error
bool cUdpServer::ErrorSession(const netid netId)
{
	return true;
}


void cUdpServer::Close()
{
	m_state = eState::Disconnect;
	if (m_thread.joinable())
		m_thread.join();

	SAFE_DELETEA(m_recvBuffer);
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
	// UDP Server only receive packet
	assert(0);
	return 1;
}


int cUdpServer::SendImmediate(const netid rcvId, const cPacket &packet)
{
	assert(0); // not implements
	return 1;
}


// send packet to all client
int cUdpServer::SendAll(const cPacket &packet, set<netid> *outErrs //= nullptr
)
{
	// udp server only receive packet
	assert(0);
	return 1;
}


// UDP Server Thread Function
unsigned WINAPI cUdpServer::ThreadFunction(cUdpServer* udp)
{
	while (eState::Connect == udp->m_state)
	{
		udp->Process();

		if (udp->m_sleepMillis)
			std::this_thread::sleep_for(std::chrono::milliseconds(udp->m_sleepMillis));
	}

	return 0;
}
