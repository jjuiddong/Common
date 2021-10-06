
#include "stdafx.h"
#include "tcpclient.h"

using namespace std;
using namespace network2;


cTcpClient::cTcpClient(
	const StrId &name //= "TcpClient"
	, const int logId //= -1
)
	: cNetworkNode(name, logId)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_sendQueue(this, logId)
	, m_recvQueue(this, logId)
	, m_recvBuffer(NULL)
	, m_isThreadMode(false)
	, m_sessionListener(nullptr)
	, m_clientSidePort(-1)
{
}

cTcpClient::~cTcpClient()
{
	Close();
	SAFE_DELETEA(m_recvBuffer);
}


bool cTcpClient::Init(const Str16 &ip
	, const int port	
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const int clientSidePort //= -1
	, const bool isThreadMode //= true
)
{
	Close();

	m_ip = ip;
	m_port = port;
	m_clientSidePort = clientSidePort;
	m_sleepMillis = sleepMillis;
	m_maxBuffLen = packetSize;
	m_isThreadMode = isThreadMode;

	m_state = eState::ReadyConnect;

	if (isThreadMode)
	{
		m_thread = std::thread(cTcpClient::ThreadFunction, this);
	}
	else
	{
		if (!ConnectServer())
		{
			goto $error;
		}
	}

	if (!m_recvQueue.Init(packetSize, maxPacketCount))
	{
		goto $error;
	}

	if (!m_sendQueue.Init(packetSize, maxPacketCount))
	{
		goto $error;
	}

	return true;


$error:
	Close();
	return false;
}


// return socket correspond netid
// client has only one netid
SOCKET cTcpClient::GetSocket(const netid netId)
{
	if ((m_id != netId) && (network2::SERVER_NETID != netId))
	{
		assert(0);
	}
	return m_socket;
}


// return netid correspond socket
// client han only one socket
netid cTcpClient::GetNetIdFromSocket(const SOCKET sock)
{
	if (m_socket != sock)
	{
		assert(0);
	}
	return m_id;
}


void cTcpClient::GetAllSocket(OUT map<netid, SOCKET> &out)
{
	common::AutoCSLock cs(m_cs);
	out.insert({ m_id, m_socket });
	out.insert({ network2::SERVER_NETID, m_socket });
}


int cTcpClient::Send(const netid rcvId, const cPacket &packet)
{
	m_sendQueue.Push(rcvId, packet);
	return 1;
}


int cTcpClient::SendImmediate(const netid rcvId, const cPacket &packet)
{
	assert(0); // not implements
	return 1;
}


// Network Packet Recv/Send
// for single thread tcpclient
bool cTcpClient::Process()
{
	RETV(eState::Connect != m_state, false);

	const timeval t = { 0, m_sleepMillis * 1000};

	if (!m_recvBuffer)
		m_recvBuffer = new char[m_maxBuffLen];

	// Receive Packet
	fd_set readSockets;
	FD_ZERO(&readSockets);
	FD_SET(m_socket, &readSockets);
	const int ret = select(readSockets.fd_count, &readSockets, NULL, NULL, &t);
	if (ret != 0 && ret != SOCKET_ERROR)
	{
		const int result = recv(readSockets.fd_array[0], m_recvBuffer, m_maxBuffLen, 0);
		if (result == SOCKET_ERROR || result == 0)
		{
			// when receive packet size == 0, disconnect state
			// socket error occur
			m_recvQueue.Push(m_id, DisconnectPacket(this, m_id));
			m_state = eState::Disconnect;
		}
		else
		{
			const netid netId = GetNetIdFromSocket(readSockets.fd_array[0]);
			if (netId == INVALID_NETID)
			{
				// not found netid
			}
			else
			{
				m_recvQueue.Push(netId, (BYTE*)m_recvBuffer, result);
			}
		}
	}
	//~Receive Packet	

	// Send Packet
	{
		map<netid, SOCKET> socks;
		GetAllSocket(socks);

		set<netid> errSocks;
		m_sendQueue.SendAll(socks, &errSocks);

		if (!errSocks.empty())
		{
			m_state = eState::Disconnect;
			m_recvQueue.Push(m_id, DisconnectPacket(this, m_id));
		}
	}

	return true;
}


// disconnect server event
bool cTcpClient::RemoveSession()
{
	if (m_sessionListener)
		m_sessionListener->RemoveSession(*this);

	Close();
	return true;
}


// update session listener
void cTcpClient::SetSessionListener(iSessionListener *listener)
{
	m_sessionListener = listener;
}


// Connect Server
bool cTcpClient::ConnectServer()
{
	if (m_state != eState::ReadyConnect)
	{
		m_state = eState::ConnectError;
		return false;
	}

	m_state = eState::TryConnect;

	if (!network2::LaunchTCPClient(m_ip.c_str(), m_port
		, m_socket, true, m_clientSidePort))
	{
		m_state = eState::Disconnect;
		dbg::Logc(2, "Error!! TCP Connection, ip=%s, port=%d\n"
			, m_ip.c_str(), m_port);
		return false;
	}

	m_state = eState::Connect;

	return true;
}


// try reconnect
bool cTcpClient::ReConnect()
{
	if (IsReadyConnect())
		return true; // already try connect

	Close();

	m_state = eState::ReadyConnect;

	if (m_isThreadMode)
	{
		m_thread = std::thread(ThreadFunction, this);
	}
	else
	{
		if (!ConnectServer())
			return false;
	}

	return true;
}


void cTcpClient::Close()
{
	m_state = eState::Disconnect;
	if (m_thread.joinable()) // wait until thread terminate
		m_thread.join();

	__super::Close();
	m_state = eState::Disconnect;
}


// TCP Client Thread Function
int cTcpClient::ThreadFunction(cTcpClient *client)
{
	if (!client->ConnectServer())
		return 0;

	while (eState::Connect == client->m_state)
	{
		client->Process();
	}

	return 0;
}
