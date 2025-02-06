
#include "stdafx.h"
#include "udpclient2.h"

using namespace std;
using namespace network2;


cUdpClient2::cUdpClient2(
	const StrId &name //= "UdpClient2"
	, const int logId //= -1
)
	: cNetworkNode(name, logId)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_sendQueue(this, logId)
	, m_recvQueue(this, logId)
	, m_isThreadMode(true)
	, m_recvBuffer(nullptr)
{
}

cUdpClient2::~cUdpClient2()
{
	Close();
	SAFE_DELETEA(m_recvBuffer);
}


bool cUdpClient2::Init(const Str16 &ip
	, const int port
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //= true
)
{
	Close();

	m_ip = ip;
	m_port = port;
	m_sleepMillis = sleepMillis;
	m_maxBuffLen = packetSize;
	m_isThreadMode = isThreadMode;

	if (!m_sendQueue.Init(packetSize, maxPacketCount))
		goto $error;

	if (!m_recvQueue.Init(packetSize, maxPacketCount))
		goto $error;

	if (network2::LaunchUDPClient(ip.c_str(), port, m_sockaddr, m_socket))
	{
		m_state = eState::Connect;
		dbg::Logc(1, "Connect UDP Client ip=%s, port=%d\n", ip.c_str(), port);
	}
	else
	{
		dbg::Logc(1, "Error!! Connect UDP Client ip=%s, port=%d\n", ip.c_str(), port);
		goto $error;
	}

	if (isThreadMode)
		m_thread = std::thread(cUdpClient2::ThreadFunction, this);
	return true;


$error:
	Close();
	return false;
}


// Network Packet Recv/Send
// for single thread tcpclient
bool cUdpClient2::Process()
{
	if (eState::Connect != m_state)
		return false;

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


// return socket correspond netid
// client has only one netid
SOCKET cUdpClient2::GetSocket(const netid netId)
{
	if ((m_id != netId) && (network2::SERVER_NETID != netId))
	{
		assert(0);
	}
	return m_socket;
}


// return netid correspond socket
// client has only one socket
netid cUdpClient2::GetNetIdFromSocket(const SOCKET sock)
{
	if (m_socket != sock)
	{
		assert(0);
	}
	return m_id;
}


void cUdpClient2::GetAllSocket(OUT map<netid, SOCKET> &out)
{
	out.insert({ m_id, m_socket });
	out.insert({ network2::SERVER_NETID, m_socket });
}


int cUdpClient2::Send(const netid rcvId, const cPacket &packet)
{
	m_sendQueue.Push(rcvId, packet);
	return 1;
}


int cUdpClient2::SendImmediate(const netid rcvId, const cPacket &packet)
{
	assert(0); // not implements
	return 1;
}


void cUdpClient2::Close()
{
	m_state = eState::Disconnect;
	if (m_thread.joinable())
		m_thread.join();

	__super::Close();
	m_state = eState::Disconnect;
}


// UDP Client Thread Function
int cUdpClient2::ThreadFunction(cUdpClient2 *udp)
{
	while (eState::Connect == udp->m_state)
	{
		if (!udp->IsConnect() || (INVALID_SOCKET == udp->m_socket))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(udp->m_sleepMillis));
			continue;
		}
		udp->Process();
		//std::this_thread::sleep_for(std::chrono::milliseconds(udp->m_sleepMillis));
	}
	return 0;
}
