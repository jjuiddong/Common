
#include "stdafx.h"
#include "udpclient.h"

using namespace std;
using namespace network2;


cUdpClient::cUdpClient(
	const StrId &name //= "UdpClient"
	, const int logId //= -1
)
	: cNetworkNode(name, logId)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_sendQueue(this, logId)
	, m_isThreadMode(true)
{
}

cUdpClient::~cUdpClient()
{
	Close();
}


bool cUdpClient::Init(const Str16 &ip
	, const int port
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_MAX_PACKETCOUNT
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

	if (network2::LaunchUDPClient(ip.c_str(), port, m_sockaddr, m_socket))
	{
		m_state = eState::Connect;
		dbg::Logc(1, "Connect UDP Client ip=%s, port=%d\n", ip.c_str(), port);

		if (!m_sendQueue.Init(packetSize, maxPacketCount))
		{
			goto $error;
		}
	}
	else
	{
		dbg::Logc(1, "Error!! Connect UDP Client ip=%s, port=%d\n", ip.c_str(), port);
		goto $error;
	}

	if (isThreadMode)
	{
		m_thread = std::thread(cUdpClient::ThreadFunction, this);
	}

	return true;


$error:
	Close();
	return false;
}


// Network Packet Recv/Send
// for single thread tcpclient
bool cUdpClient::Process()
{
	if (eState::Connect != m_state)
		return false;

	m_sendQueue.SendAll(m_sockaddr);
	return true;
}


// netId에 해당하는 socket을 리턴한다.
// 클라이언트는 netid를 한개 만 관리한다. 
SOCKET cUdpClient::GetSocket(const netid netId)
{
	if ((m_id != netId) && (network2::SERVER_NETID != netId))
	{
		assert(0);
	}
	return m_socket;
}


// 클라이언트는 SOCKET을 한개 만 관리한다. 
netid cUdpClient::GetNetIdFromSocket(const SOCKET sock)
{
	if (m_socket != sock)
	{
		assert(0);
	}
	return m_id;
}


void cUdpClient::GetAllSocket(OUT map<netid, SOCKET> &out)
{
	out.insert({ m_id, m_socket });
	out.insert({ network2::SERVER_NETID, m_socket });
}


int cUdpClient::Send(const netid rcvId, const cPacket &packet)
{
	m_sendQueue.Push(rcvId, packet);
	return 1;
}


int cUdpClient::SendImmediate(const netid rcvId, const cPacket &packet)
{
	assert(0); // not implements
	return 1;
}


void cUdpClient::Close()
{
	m_state = eState::Disconnect;
	if (m_thread.joinable()) // 쓰레드 종료.
		m_thread.join();

	Sleep(100);
	cNetworkNode::Close();
	m_state = eState::Disconnect;
}


// UDP Client Thread Function
int cUdpClient::ThreadFunction(cUdpClient *udp)
{
	while (eState::Connect == udp->m_state)
	{
		if (!udp->IsConnect() || (INVALID_SOCKET == udp->m_socket))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(udp->m_sleepMillis));
			continue;
		}
		udp->Process();
		std::this_thread::sleep_for(std::chrono::milliseconds(udp->m_sleepMillis));
	}
	return 0;
}
