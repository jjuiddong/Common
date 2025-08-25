
#include "stdafx.h"
#include "webclient.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/PollSet.h"

using namespace std;
using namespace network2;


cWebClient::cWebClient( 
	const StrId &name //= "WebClient"
	, const int logId //= -1
)
	: cNetworkNode(name, logId)
	, m_session(nullptr)
	, m_request(nullptr)
	, m_response(nullptr)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_sendQueue(this, logId)
	, m_recvQueue(this, logId)
	, m_recvBuffer(nullptr)
	, m_sendBuffer(nullptr)
	, m_isThreadMode(false)
	, m_sessionListener(nullptr)
{
}

cWebClient::~cWebClient()
{
	Close();
	SAFE_DELETEA(m_recvBuffer);
	SAFE_DELETEA(m_sendBuffer);
}


bool cWebClient::Init(const string &url
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //= true
)
{
	Close();

	// parse url, port
	vector<string> toks;
	common::tokenizer(url, ":", "", toks);
	if (toks.size() >= 2)
	{
		m_url = "";
		for (int i = 0; i < (int)toks.size() - 1; ++i)
		{
			m_url += toks[i];
			if (i != (int)toks.size() - 2)
				m_url += ":";
		}
		m_port = atoi(toks.back().c_str());
	}
	else {
		m_url = url;
		m_port = 80;
	}

	m_sleepMillis = sleepMillis;
	m_maxBuffLen = packetSize + 14; //+14 websocket header
	m_isThreadMode = isThreadMode;

	m_state = eState::ReadyConnect;

	if (!m_recvQueue.Init(packetSize, maxPacketCount))
		goto $error;
	if (!m_sendQueue.Init(packetSize, maxPacketCount))
		goto $error;

	if (isThreadMode)
	{
		m_thread = std::thread(cWebClient::ThreadFunction, this);
	}
	else
	{
		if (!ConnectServer())
			goto $error;
	}

	return true;


$error:
	Close();
	return false;
}


// return socket correspond netid
// webclient has one socket
SOCKET cWebClient::GetSocket(const netid netId)
{
	if ((m_id != netId) && (network2::SERVER_NETID != netId))
	{
		assert(0);
	}
	return m_socket;
}


// return netid correspond socket
// webclient has one netid
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
	out.insert({ m_id, 0});
	out.insert({ network2::SERVER_NETID, 0 });
}


int cWebClient::Send(const netid rcvId, const cPacket &packet)
{
	m_sendQueue.Push(rcvId, packet);
	return 1;
}


int cWebClient::SendImmediate(const netid rcvId, const cPacket &packet)
{
	assert(0); // not implements
	return 1;
}


// default send
int cWebClient::SendPacket(const SOCKET sock, const cPacket &packet)
{
	int result = 0;
	if (m_websocket)
	{
		try {
			// upgrade sendFrame() -> sendFrame2(), to avoid memory alloc
			result = m_websocket->sendFrame2(m_sendBuffer, m_maxBuffLen
				, packet.m_data, packet.GetPacketSize()
				, Poco::Net::WebSocket::FRAME_BINARY);
		}
		catch (std::exception &e) {
			// nothing~
			dbg::Logc(2, "websocket client send exception %s\n", e.what());
		}
	}
	return result;
}


// Network Packet Recv/Send
// for single thread tcpclient
bool cWebClient::Process()
{
	using Poco::Net::PollSet;

	if (eState::Connect != m_state)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepMillis));
		return false;
	}

	if (!m_recvBuffer)
		m_recvBuffer = new char[m_maxBuffLen];
	if (!m_sendBuffer)
		m_sendBuffer = new char[m_maxBuffLen];

	// receive packet
	PollSet ps;
	ps.add(*m_websocket, PollSet::POLL_READ);
	//Poco::Net::Socket::SocketList reads;
	//Poco::Net::Socket::SocketList writes;
	//Poco::Net::Socket::SocketList excepts;
	//reads.push_back(*m_websocket);

	int selResult = 0;
	try
	{
		PollSet::SocketModeMap sm = ps.poll(Poco::Timespan(0, m_sleepMillis * 1000));
		for (const auto& s : sm)
		{
			if (s.second & PollSet::POLL_READ) 
				++selResult;
			if (s.second & PollSet::POLL_ERROR)
			{
				selResult = SOCKET_ERROR;
				break;
			}
		}
		//selResult = m_websocket->select(reads, writes, excepts
		//	, Poco::Timespan(0, m_sleepMillis * 1000));
	}
	catch (Poco::TimeoutException)
	{
		// timeout
	}
	catch (std::exception &e)
	{
		dbg::Logc(2, "error cWebClient select(), %s\n", e.what());
	}

	if ((0 != selResult) && (SOCKET_ERROR != selResult))
	{
		int result = 0;
		try
		{
			int flags = 0;
			result = m_websocket->receiveFrame(m_recvBuffer, m_maxBuffLen, flags);
			if (flags & Poco::Net::WebSocket::FRAME_OP_CLOSE)
				result = (int)INVALID_SOCKET;
		}
		catch (Poco::TimeoutException)
		{
			// timeout
		}
		catch (std::exception &e)
		{
			// socket error occurred!!
			dbg::Logc(2, "error cWebClient receive, %s\n", e.what());
			result = (int)SOCKET_ERROR;
		}

		if (((int)SOCKET_ERROR == result) || (0 == result)) // connection error, disconnect
		{
			m_recvQueue.Push(m_id, DisconnectPacket(this, m_id));
			m_state = eState::Disconnect;
		}
		else if (result > 0)
		{
			const netid netId = GetNetIdFromSocket(m_socket);
			if (INVALID_NETID == netId)
			{
				// not found netid
			}
			else
			{
				m_recvQueue.Push(netId, (BYTE*)m_recvBuffer, result);
			}
		}
	}

	// Send Packet
	{
		map<netid, SOCKET> socks;
		GetAllSocket(socks);

		set<netid> errNetIds;
		m_sendQueue.SendAll(socks, &errNetIds, &m_protocolListeners);

		if (!errNetIds.empty())
		{
			m_state = eState::Disconnect;
			m_recvQueue.Push(m_id, DisconnectPacket(this, m_id));
		}
	}
	return true;
}


// connect server event, call from basic dispatcher
bool cWebClient::AddSession()
{
	if (m_sessionListener)
		m_sessionListener->AddSession(*this);
	return true;
}


// disconnect server event, call from basic dispatcher
bool cWebClient::RemoveSession()
{
	if (m_sessionListener)
		m_sessionListener->RemoveSession(*this);

	Close();
	return true;
}


void cWebClient::SetSessionListener(iSessionListener *listener)
{
	m_sessionListener = listener;
}


// Connect Server
bool cWebClient::ConnectServer()
{
	if (eState::ReadyConnect != m_state)
	{
		m_state = eState::ConnectError;
		return false;
	}

	m_state = eState::TryConnect;

	m_session = new Poco::Net::HTTPClientSession(m_url, m_port);
	m_request = new Poco::Net::HTTPRequest(Poco::Net::HTTPRequest::HTTP_GET
		, "/?encoding=text", Poco::Net::HTTPMessage::HTTP_1_1);
	m_request->set("origin", "http://www.websocket.org");
	m_response = new Poco::Net::HTTPResponse;

	bool isConnection = false;

	try
	{
		m_websocket = new Poco::Net::WebSocket(*m_session, *m_request, *m_response);
		m_websocket->setReceiveTimeout(Poco::Timespan(0, m_sleepMillis * 1000));
		m_recvQueue.Push(m_id, ConnectPacket(this, m_id));
		isConnection = true;
	}
	catch (std::exception &e)
	{
		m_recvQueue.Push(m_id, DisconnectPacket(this, m_id));
		dbg::Logc(2, "Error!! WebClient Connection, url=%s, port=%d, desc=%s\n"
			, m_url.c_str(), m_port, e.what());
	}

	m_state = isConnection? eState::Connect : eState::Disconnect;
	return isConnection;
}


// try reconnect
bool cWebClient::ReConnect()
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


void cWebClient::Close()
{
	m_state = eState::Disconnect;
	if (m_thread.joinable()) // wait thread terminate
		m_thread.join();

	if (m_websocket)
		m_websocket->close();
	if (m_session)
		m_session->reset();
	if (m_request)
		m_request->clear();
	if (m_response)
		m_response->clear();

	SAFE_DELETE(m_websocket);
	SAFE_DELETE(m_session);
	SAFE_DELETE(m_request);
	SAFE_DELETE(m_response);

	__super::Close();
	m_state = eState::Disconnect;
}


// TCP Client Thread Function
int cWebClient::ThreadFunction(cWebClient *client)
{
	if (!client->ConnectServer())
		return 0; // error connection, stop thread
	while (eState::Connect == client->m_state)
	{
		client->Process();
	}
	return 0;
}
