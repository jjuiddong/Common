
#include "stdafx.h"
#include "webclient.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/HTTPClientSession.h"

using namespace std;
using namespace network2;


cWebClient::cWebClient( const bool isPacketLog //= false
	, const StrId &name //= "WebClient"
)
	: cNetworkNode(name, isPacketLog)
	, m_session(nullptr)
	, m_request(nullptr)
	, m_response(nullptr)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_sendQueue(this, isPacketLog)
	, m_recvQueue(this, isPacketLog)
	, m_recvBuffer(nullptr)
	, m_isThreadMode(false)
{
}

cWebClient::~cWebClient()
{
	Close();
}


bool cWebClient::Init(const string &url
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_MAX_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //= true
)
{
	Close();

	// parse url, port
	vector<string> toks;
	common::tokenizer(url, ":", "", toks);
	if (toks.size() == 2)
	{
		m_url = toks[0];
		m_port = atoi(toks[1].c_str());
	}
	else {
		m_url = url;
		m_port = 80;
	}

	m_sleepMillis = sleepMillis;
	m_maxBuffLen = packetSize;
	m_isThreadMode = isThreadMode;

	m_state = READYCONNECT;

	if (isThreadMode)
	{
		m_thread = std::thread(cWebClient::ThreadFunction, this);
	}
	else
	{
		if (!ConnectServer())
			goto $error;
	}

	if (!m_recvQueue.Init(packetSize, maxPacketCount))
		goto $error;
	if (!m_sendQueue.Init(packetSize, maxPacketCount))
		goto $error;

	return true;


$error:
	Close();
	return false;
}


// netId에 해당하는 socket을 리턴한다.
// 클라이언트는 netid를 한개 만 관리한다. 
SOCKET cWebClient::GetSocket(const netid netId)
{
	if ((m_id != netId) && (network2::SERVER_NETID != netId))
	{
		assert(0);
	}
	return m_socket;
}


// 클라이언트는 SOCKET을 한개 만 관리한다. 
netid cWebClient::GetNetIdFromSocket(const SOCKET sock)
{
	if (m_socket != sock)
	{
		assert(0);
	}
	return m_id;
}


void cWebClient::GetAllSocket(OUT map<netid, SOCKET> &out)
{
	// add temporal socket value to process socket send
	common::AutoCSLock cs(m_cs);
	out.insert({ m_id, 0});
	out.insert({ network2::SERVER_NETID, 0 });
}


int cWebClient::Send(const netid rcvId, const cPacket &packet)
{
	m_sendQueue.Push(rcvId, packet);
	return 1;
}


// default send
int cWebClient::SendPacket(const SOCKET sock, const cPacket &packet)
{
	int result = 0;
	if (m_websocket && m_packetHeader)
	{
		try {
			result = m_websocket->sendFrame(packet.m_data
				, packet.GetPacketSize(), Poco::Net::WebSocket::FRAME_BINARY);
		}
		catch (std::exception &e) {
			// nothing~
			dbg::Logc(2, "websocket exception %s\n", e.what());
		}
	}
	return result;
}


// Network Packet Recv/Send
// for single thread tcpclient
bool cWebClient::Process()
{
	if (CONNECT != m_state)
		return false;

	if (!m_recvBuffer)
		m_recvBuffer = new char[m_maxBuffLen];

	// Receive Packet
	int result = 0;
	try
	{
		int flags = 0;
		result = m_websocket->receiveFrame(m_recvBuffer, m_maxBuffLen, flags);
	}
	catch (Poco::TimeoutException)
	{
		// timeout
	}
	catch (std::exception)
	{
		// error occurred!!
		result = SOCKET_ERROR;
	}

	if (result == SOCKET_ERROR) // connection error, disconnect
	{
		m_state = cWebClient::DISCONNECT;
	}
	else if (result > 0)
	{
		const netid netId = GetNetIdFromSocket(m_socket);
		if (netId == INVALID_NETID)
		{
			// not found netid
		}
		else
		{
			if (!m_packetHeader)
				return false; // internal error occurred!!

			m_recvQueue.Push(netId, (BYTE*)m_recvBuffer, result);
		}
	}

	// Send Packet
	{
		map<netid, SOCKET> socks;
		GetAllSocket(socks);

		set<netid> errSocks;
		m_sendQueue.SendAll(socks, &errSocks);

		if (!errSocks.empty())
			m_state = cWebClient::DISCONNECT;
	}
	return true;
}


// Connect Server
bool cWebClient::ConnectServer()
{
	if (m_state != cWebClient::READYCONNECT)
	{
		m_state = cWebClient::CONNECT_ERROR;
		return false;
	}

	m_state = cWebClient::TRYCONNECT;

	m_session = new Poco::Net::HTTPClientSession(m_url, m_port);
	m_request = new Poco::Net::HTTPRequest(Poco::Net::HTTPRequest::HTTP_GET
		, "/?encoding=text", Poco::Net::HTTPMessage::HTTP_1_1);
	m_request->set("origin", "http://www.websocket.org");
	m_response = new Poco::Net::HTTPResponse;

	try 
	{
		m_websocket = new Poco::Net::WebSocket(*m_session, *m_request, *m_response);
		m_websocket->setReceiveTimeout(Poco::Timespan(0, m_sleepMillis * 1000));
	}
	catch (std::exception &)
	{
		m_state = cWebClient::DISCONNECT;
		dbg::Logc(2, "Error!! WebClient Connection, url=%s, port=%d\n"
			, m_url.c_str(), m_port);
		return false;
	}

	m_state = cWebClient::CONNECT;

	return true;
}


bool cWebClient::ReConnect()
{
	if (IsReadyConnect())
		return false; // already try connect

	Close();

	m_state = READYCONNECT;

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


void cWebClient::Close()
{
	m_state = DISCONNECT;
	if (m_thread.joinable()) // wait thread terminate
		m_thread.join();

	if (m_websocket)
		m_websocket->close();

	SAFE_DELETE(m_websocket);
	SAFE_DELETE(m_session);
	SAFE_DELETE(m_request);
	SAFE_DELETE(m_response);

	cNetworkNode::Close();
	SAFE_DELETEA(m_recvBuffer);
	m_state = DISCONNECT;
}


// TCP Client Thread Function
int cWebClient::ThreadFunction(cWebClient *client)
{
	if (!client->ConnectServer())
		return 0; // error connection, stop thread

	while (cWebClient::CONNECT == client->m_state)
	{
		client->Process();
	}

	return 0;
}
