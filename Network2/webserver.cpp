
#include "stdafx.h"
#include "webserver.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/HTTPClientSession.h"


using namespace network2;
using Poco::Net::ServerSocket;
using Poco::Net::WebSocket;
using Poco::Net::WebSocketException;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;


//----------------------------------------------------------------------------------
// cWebSocketRequestHandler
class cWebSocketRequestHandler : public HTTPRequestHandler
{
public:
	cWebServer &m_webServer;
	cWebSocketRequestHandler(cWebServer &wsServer) : m_webServer(wsServer) {}

	// WebSocket Accept Process
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
		try
		{
			// tricky code, create temporal websession and then register cWebServer
			// server accept proccess
			const int uid = common::GenerateId();
			WebSocket *ws = new WebSocket(request, response);
			ws->setReceiveTimeout(Poco::Timespan(0, 0)); // 0 micro seconds
			cWebSession *session = new cWebSession(uid, "", ws);
			{
				AutoCSLock cs(m_webServer.m_cs);
				m_webServer.m_tempSessions.push_back(session);
			}
			const string clientAddr = ws->address().toString();
			const int port = ws->address().port();
			m_webServer.m_recvQueue.Push(SERVER_NETID
				, network2::AcceptPacket(&m_webServer, uid, clientAddr, port));
		}
		catch (WebSocketException& e)
		{
			switch (e.code())
			{
			case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
				response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
				// fallthrough
			case WebSocket::WS_ERR_NO_HANDSHAKE:
			case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
			case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
				response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
				response.setContentLength(0);
				response.send();
				break;
			}
		}
	}
};


//----------------------------------------------------------------------------------
// cWebServer
cWebServer::cWebServer(
	cWebSessionFactory *sessionFactory //= new cWebSessionFactory()
	, const StrId &name //= "TcpServer"
	, const int logId //= -1
)
	: cNetworkNode(name, logId)
	, m_httpServer(nullptr)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_sessionFactory(sessionFactory)
	, m_sendQueue(this, logId)
	, m_recvQueue(this, logId)
	, m_sessionListener(nullptr)
	, m_recvBuffer(nullptr)
	, m_lastAcceptTime(0)
	, m_isThreadMode(true)
{
}

cWebServer::~cWebServer()
{
	Close();
}


// initialize websocket server
bool cWebServer::Init(const int bindPort
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_MAX_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //= true
)
{
	Close();

	m_ip = "localhost";
	m_port = bindPort;
	m_sleepMillis = sleepMillis;
	m_maxBuffLen = packetSize;
	m_isThreadMode = isThreadMode;
	m_timer.Create();

	m_websocket = new Poco::Net::ServerSocket(bindPort);
	m_httpServer = new Poco::Net::HTTPServer(this, *m_websocket, new HTTPServerParams);
	m_httpServer->start();

	m_state = eState::Connect;
	dbg::Logc(1, "Bind WebSocket Server port = %d\n", bindPort);

	if (!m_recvQueue.Init(packetSize, maxPacketCount))
	{
		goto $error;
	}

	if (!m_sendQueue.Init(packetSize, maxPacketCount))
	{
		goto $error;
	}

	if (isThreadMode)
	{
		m_thread = std::thread(cWebServer::ThreadFunction, this);
	}

	return true;


$error:
	Close();
	return false;
}


// Network Packet Recv/Send
// for single thread
bool cWebServer::Process()
{
	if (!m_recvBuffer)
		m_recvBuffer = new char[m_maxBuffLen];

	ReceiveProcces();

	// Send Packet
	{
		set<netid> errNetis;
		m_sendQueue.SendAll(&errNetis);

		for (auto id : errNetis)
			m_recvQueue.Push(id, DisconnectPacket(this, id));
	}

	return true;
}


// add session
bool cWebServer::AddSession(const SOCKET sock, const Str16 &ip, const int port)
{
	RETV(!m_sessionFactory, false);

	if (FindSessionBySocket(sock))
		return false; // Error!! Already Exist

	common::AutoCSLock cs(m_cs);

	// find temporal session by socket(netid), tricky code
	cWebSession *tmpSession = nullptr;
	for (auto &session : m_tempSessions)
		if (sock == session->m_id)
		{
			tmpSession = session;
			break;
		}
	if (!tmpSession)
		return false; // not found temporal session

	cWebSession *session = m_sessionFactory->New();
	session->m_id = tmpSession->m_id;
	session->m_socket = 0; // not use socket
	session->m_ws = tmpSession->m_ws;
	session->m_ip = ip;
	session->m_port = port;
	session->m_state = eState::Connect;
	m_sessions.insert({ session->m_id, session });

	if (m_logId >= 0)
		network2::LogSession(m_logId, *session);

	if (m_sessionListener)
		m_sessionListener->AddSession(*session);

	tmpSession->m_ws = nullptr; // no delete websocket, initialize
	common::removevector(m_tempSessions, tmpSession);
	SAFE_DELETE(tmpSession);
	return true;
}


// Remove Session
bool cWebServer::RemoveSession(const netid netId)
{
	cSession *session = NULL;
	{
		common::AutoCSLock cs(m_cs);

		auto it = m_sessions.find(netId);
		if (m_sessions.end() == it)
			return false; // not found session
		session = it->second;
	}

	if (m_sessionListener)
		m_sessionListener->RemoveSession(*session);

	{
		common::AutoCSLock cs(m_cs);

		const SOCKET sock = session->m_socket;
		session->Close();
		SAFE_DELETE(session);
		m_sessions.remove(netId);
		m_sockets.remove(sock);
	}

	return true;
}


// check exist session correspond netid
bool cWebServer::IsExist(const netid netId)
{
	common::AutoCSLock cs(m_cs);

	auto it = m_sessions.find(netId);
	return m_sessions.end() != it;
}


cSession* cWebServer::FindSessionBySocket(const SOCKET sock)
{
	return nullptr;
}


cSession* cWebServer::FindSessionByNetId(const netid netId)
{
	common::AutoCSLock cs(m_cs);

	auto it = m_sessions.find(netId);
	if (m_sessions.end() == it)
		return NULL; // not found session
	return it->second;
}


cSession* cWebServer::FindSessionByName(const StrId &name)
{
	common::AutoCSLock cs(m_cs);

	for (auto &session : m_sessions.m_seq)
		if (session->m_name == name)
			return session;
	return NULL; // not found session
}


// packet receive process
bool cWebServer::ReceiveProcces()
{
	common::AutoCSLock cs(m_cs);

	set<netid> rmSessions; // remove session ids
	for (auto &session : m_sessions.m_seq)
	{
		int result = 0;
		try
		{
			int flags = 0;
			result = session->m_ws->receiveFrame(m_recvBuffer, m_maxBuffLen, flags);
			if (flags & Poco::Net::WebSocket::FRAME_OP_CLOSE)
				result = INVALID_SOCKET;
		}
		catch (Poco::TimeoutException)
		{
			// timeout
		}
		catch (std::exception)
		{
			// connection error
			result = INVALID_SOCKET;
		}

		if (INVALID_SOCKET == result)
		{
			// disconnect session
			if (!m_recvQueue.Push(session->m_id, DisconnectPacket(this, session->m_id)))
				rmSessions.insert(session->m_id); // exception process
		}

		if (result > 0)
		{
			m_recvQueue.Push(session->m_id, (BYTE*)m_recvBuffer, result);
		}
	}

	// remove session
	for (auto &id : rmSessions)
		RemoveSession(id);
	return true;
}

void cWebServer::SetSessionListener(iSessionListener *listener)
{
	m_sessionListener = listener;
}


// return soket correspond netid
SOCKET cWebServer::GetSocket(const netid netId)
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
netid cWebServer::GetNetIdFromSocket(const SOCKET sock)
{
	if (sock == m_socket)
		return m_id;

	{
		common::AutoCSLock cs(m_cs);
		auto it = m_sockets.find(sock);
		if (it == m_sockets.end())
			return INVALID_NETID;
		return it->second->m_id;
	}
	return INVALID_NETID;
}


void cWebServer::GetAllSocket(OUT map<netid, SOCKET> &out)
{
	common::AutoCSLock cs(m_cs);
	for (auto &sock : m_sockets.m_seq)
		out.insert({ sock->m_id, sock->m_socket });
}


// send packet 
int cWebServer::Send(const netid rcvId, const cPacket &packet)
{
	m_sendQueue.Push(rcvId, packet);
	return 1;
}


// send packet immediatly
int cWebServer::SendImmediate(const netid rcvId, const cPacket &packet)
{
	auto it = m_sessions.find(rcvId);
	if (m_sessions.end() == it)
		return 0;

	int result = 0;
	cWebSession *session = it->second;
	if (session->m_ws)
	{
		try {
			result = session->m_ws->sendFrame(packet.m_data
				, packet.GetPacketSize(), Poco::Net::WebSocket::FRAME_BINARY);
		}
		catch (std::exception &e) {
			dbg::Logc(2, "websocket client send exception %s\n", e.what());
		}
	}
	return result;
}


// send packet to all client
int cWebServer::SendAll(const cPacket &packet)
{
	assert(0);
	return 1; // not implements
}


// httprequesthandler factory hanlder
Poco::Net::HTTPRequestHandler* cWebServer::createRequestHandler(
	const Poco::Net::HTTPServerRequest& request)
{
	if (request.find("Upgrade") != request.end()
		&& Poco::icompare(request["Upgrade"], "websocket") == 0)
		return new cWebSocketRequestHandler(*this);
	return nullptr;
}


// clear
void cWebServer::Close()
{
	if (m_httpServer)
	{
		m_httpServer->stop();
		if (m_websocket)
			m_websocket->close();
		SAFE_DELETE(m_websocket);
		SAFE_DELETE(m_httpServer);
	}
}


// WebSocket Server Thread Function
unsigned WINAPI cWebServer::ThreadFunction(cWebServer* server)
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
