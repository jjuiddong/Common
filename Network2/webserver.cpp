
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
			WebSocket *ws = new WebSocket(request, response);
			cWebSession *session = new cWebSession(common::GenerateId(), "", ws);
			{
				AutoCSLock cs(m_webServer.m_cs);
				m_webServer.m_tempSessions.push_back(session);
			}
			vector<string> toks;
			const string addr = ws->address().toString();
			common::tokenizer(addr, ":", "", toks);
			const string clientAddr = (toks.size() == 2) ? toks[0] : addr;
			const int port = ws->address().port();
			m_webServer.m_recvQueue.Push(SERVER_NETID
				, network2::AcceptPacket(&m_webServer, ws->impl()->sockfd(), clientAddr, port));
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
// HTTPRequestHandlerFactory
class cHTTPRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	cWebServer &m_webServer;
	cHTTPRequestHandlerFactory(cWebServer &wsServer) : m_webServer(wsServer) {
	}
	Poco::Net::HTTPRequestHandler* createRequestHandler(
		const Poco::Net::HTTPServerRequest& request)
	{
		if (request.find("Upgrade") != request.end()
			&& Poco::icompare(request["Upgrade"], "websocket") == 0)
			return new cWebSocketRequestHandler(m_webServer);
		return nullptr;
	}
};


//----------------------------------------------------------------------------------
// cWebServer
cWebServer::cWebServer(
	cWebSessionFactory *sessionFactory //= new cWebSessionFactory()
	, const StrId &name //= "WebServer"
	, const int logId //= -1
)
	: cNetworkNode(name, logId)
	, m_websocket(nullptr)
	, m_httpServer(nullptr)
	, m_maxBuffLen(RECV_BUFFER_LENGTH)
	, m_sleepMillis(10)
	, m_sessionFactory(sessionFactory)
	, m_sendQueue(this, logId)
	, m_recvQueue(this, logId)
	, m_sessionListener(nullptr)
	, m_recvBuffer(nullptr)
	, m_sendBuffer(nullptr)
	, m_isThreadMode(true)
	, m_isUpdateSocket(false)
{
}

cWebServer::~cWebServer()
{
	Close();

	SAFE_DELETE(m_sessionFactory);
	SAFE_DELETEA(m_recvBuffer);
	SAFE_DELETEA(m_sendBuffer);
}


// initialize websocket server
// isSpawnHttpSvr: create and start http server?
bool cWebServer::Init(const int bindPort
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //= true
	, const bool isSpawnHttpSvr //= true
)
{
	Close();

	m_ip = "localhost";
	m_port = bindPort;
	m_sleepMillis = sleepMillis;
	m_maxBuffLen = packetSize + 14; //+14 websocket header
	m_isThreadMode = isThreadMode;
	m_timer.Create();

	if (!m_recvQueue.Init(packetSize, maxPacketCount))
		goto $error;
	if (!m_sendQueue.Init(packetSize, maxPacketCount))
		goto $error;

	if (isSpawnHttpSvr)
	{
		try
		{
			m_websocket = new Poco::Net::ServerSocket(bindPort);
			m_httpServer = new Poco::Net::HTTPServer( new cHTTPRequestHandlerFactory(*this)
				, *m_websocket, new HTTPServerParams);
			m_httpServer->start(); // http server thread start
		}
		catch (std::exception &e)
		{
			dbg::Logc(1, "Error Bind WebSocket Server port=%d, msg=%s\n"
				, bindPort, e.what());
			m_state = eState::Disconnect;
			return false;
		}

		dbg::Logc(1, "Bind WebSocket Server port = %d\n", bindPort);
	}
	else
	{
		dbg::Logc(1, "Start WebSocket Server with No HttpServer\n");
	}

	m_state = eState::Connect;

	if (isThreadMode)
		m_thread = std::thread(cWebServer::ThreadFunction, this);
	return true;


$error:
	Close();
	return false;
}


// Network Packet Recv/Send
// for single thread
bool cWebServer::Process()
{
	RETV(eState::Connect != m_state, false);

	if (!m_recvBuffer)
		m_recvBuffer = new char[m_maxBuffLen];
	if (!m_sendBuffer)
		m_sendBuffer = new char[m_maxBuffLen];

	cTimer time;
	time.Create();

	ReceiveProcces();

	// Send Packet
	{
		common::AutoCSLock cs(m_cs);

		set<netid> errNetIds;
		m_sendQueue.SendAll(&errNetIds);

		for (auto id : errNetIds)
			m_recvQueue.Push(id, DisconnectPacket(this, id));
	}

	return true;
}


// add session, call from webserver basic protocol
bool cWebServer::AddSession(const SOCKET sock, const Str16 &ip, const int port)
{
	RETV(!m_sessionFactory, false);

	if (FindSessionBySocket(sock))
		return false; // Error!! Already Exist

	common::AutoCSLock cs(m_cs);

	// find temporal session by socket(netid), tricky code
	cWebSession *tmpSession = nullptr;
	for (auto &session : m_tempSessions)
		if (sock == session->m_socket)
		{
			tmpSession = session;
			break;
		}
	if (!tmpSession)
		return false; // not found temporal session

	Poco::Net::WebSocket *ws = tmpSession->m_ws;
	ws->setReceiveTimeout(Poco::Timespan(0, 1000)); // 1 milliseconds
	//ws->setBlocking(false); blocking mode

	cWebSession *session = m_sessionFactory->New();
	session->m_id = tmpSession->m_id;
	session->m_socket = sock;
	session->m_ws = ws;
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

	tmpSession->m_ws = nullptr; // clear, move to new session
	tmpSession->m_socket = INVALID_SOCKET; // clear, move to new session
	common::removevector(m_tempSessions, tmpSession);
	SAFE_DELETE(tmpSession);
	return true;
}


// Remove Session
bool cWebServer::RemoveSession(const netid netId)
{
	cWebSession *session = nullptr;
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


// move session to another network node
// tricky code to share websession
// poco library httpserver always run thread mode
// many webserver has many multi threading and slow program
// so, one httpserver running and accept websession
// websession move another network node
cWebSession* cWebServer::MoveSession(const netid netId)
{
	cWebSession *session = nullptr;
	common::AutoCSLock cs(m_cs);
	auto it = m_sessions.find(netId);
	if (m_sessions.end() == it)
		return nullptr; // not found session
	session = it->second;

	const SOCKET sock = session->m_socket;
	m_sessions.remove(netId);
	m_sessions2.remove(sock);
	m_recvQueue.Remove(netId);
	m_isUpdateSocket = true;
	FD_CLR(sock, &m_sockets);
	return session;
}


// insert external session
// tricky code, no httpserver mode need to insert external session
bool cWebServer::InsertSession(cWebSession *session)
{
	RETV(!m_sessionFactory, false);

	const SOCKET sock = session->m_socket;

	if (FindSessionBySocket(sock))
		return false; // Error!! Already Exist

	common::AutoCSLock cs(m_cs);

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


// update logId
void cWebServer::SetLogId(const int logId)
{
	m_logId = logId;
	m_sendQueue.SetLogId(logId);
	m_recvQueue.SetLogId(logId);
}


// check exist session correspond netid
bool cWebServer::IsExist(const netid netId)
{
	common::AutoCSLock cs(m_cs);

	auto it = m_sessions.find(netId);
	return m_sessions.end() != it;
}


cWebSession* cWebServer::FindSessionBySocket(const SOCKET sock)
{
	common::AutoCSLock cs(m_cs);

	auto it = m_sessions2.find(sock);
	if (m_sessions2.end() == it)
		return NULL; // not found session
	return it->second;
}


cWebSession* cWebServer::FindSessionByNetId(const netid netId)
{
	common::AutoCSLock cs(m_cs);

	auto it = m_sessions.find(netId);
	if (m_sessions.end() == it)
		return NULL; // not found session
	return it->second;
}


cWebSession* cWebServer::FindSessionByName(const StrId &name)
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
	if (m_isUpdateSocket)
	{
		common::AutoCSLock cs(m_cs);
		m_readSockets = m_sockets;
		m_isUpdateSocket = false;
	}
	if (0 == m_readSockets.fd_count)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepMillis));
		return true;
	}

	set<netid> rmSessions; // remove session ids
	const timeval t = { 0, m_sessions.empty() ? 10000 : m_sleepMillis * 1000 };
	const fd_set &sockets = m_readSockets;
	fd_set readSockets = m_readSockets;

	const int selResult = select(readSockets.fd_count, &readSockets, nullptr, nullptr, &t);
	if (0 == selResult)
		return true; // nothing
	if (SOCKET_ERROR == selResult)
		return false; // error occurred, nothing

	{
		common::AutoCSLock cs(m_cs);
		for (u_int i = 0; i < sockets.fd_count; ++i)
		{
			if (!FD_ISSET(sockets.fd_array[i], &readSockets))
				continue;
			auto it = m_sessions2.find(sockets.fd_array[i]);
			if (m_sessions2.end() == it)
				continue; // not found session

			cWebSession *session = it->second;
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
			catch (std::exception &e)
			{
				dbg::Logc(1, "cWebServer receive exception netid:%d, %s\n"
					, session->m_id, e.what());
				result = INVALID_SOCKET; // connection error
			}

			if ((INVALID_SOCKET == result) || (0 == result))
			{
				// disconnect session
				session->m_state = eState::Disconnect;
				if (!m_recvQueue.Push(session->m_id, DisconnectPacket(this, session->m_id)))
					rmSessions.insert(session->m_id); // exception process
			}

			if (result > 0)
				m_recvQueue.Push(session->m_id, (BYTE*)m_recvBuffer, result);
		}//~for
	}

	// exception process, remove session
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
		auto it = m_sessions2.find(sock);
		if (it == m_sessions2.end())
			return INVALID_NETID;
		return it->second->m_id;
	}
	return INVALID_NETID;
}


void cWebServer::GetAllSocket(OUT map<netid, SOCKET> &out)
{
	common::AutoCSLock cs(m_cs);
	for (auto &sock : m_sessions2.m_seq)
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
			// upgrade sendFrame() -> sendFrame2(), to avoid memory alloc
			result = session->m_ws->sendFrame2(m_sendBuffer, m_maxBuffLen
				, packet.m_data, packet.GetPacketSize()
				, Poco::Net::WebSocket::FRAME_BINARY);
		}
		catch (std::exception &e) {
			dbg::Logc(1, "cWebServer send exception1, %s\n", e.what());
		}
	}
	return result;
}


// send packet to all client
int cWebServer::SendAll(const cPacket &packet, set<netid> *outErrs //= nullptr
)
{
	for (auto &session : m_sessions.m_seq)
	{
		if (!session->m_ws)
			continue;
		int result = 0;
		try {
			// upgrade sendFrame() -> sendFrame2(), to avoid memory alloc
			result = session->m_ws->sendFrame2(m_sendBuffer, m_maxBuffLen
				, packet.m_data, packet.GetPacketSize()
				, Poco::Net::WebSocket::FRAME_BINARY);
		}
		catch (std::exception &e) {
			dbg::Logc(1, "cWebServer send exception2, %s\n", e.what());
			if (outErrs)
				outErrs->insert(session->m_id);
		}
	}
	return 1;
}


// clear
void cWebServer::Close()
{
	m_state = eState::Disconnect;
	if (m_thread.joinable())
		m_thread.join();

	{
		AutoCSLock cs(m_cs);
		for (auto &session : m_sessions.m_seq)
		{
			if (m_sessionListener)
				m_sessionListener->RemoveSession(*session);
			SAFE_DELETE(session);
		}
		m_sessions.clear();
		m_sessions2.clear();

		for (auto &session : m_tempSessions)
			SAFE_DELETE(session);
		m_tempSessions.clear();
	}

	if (m_httpServer)
	{
		m_httpServer->stopAll();
		if (m_websocket)
			m_websocket->close();
		SAFE_DELETE(m_websocket);
		SAFE_DELETE(m_httpServer);
	}

	FD_ZERO(&m_sockets);
	FD_ZERO(&m_readSockets);	
	__super::Close();
}


// WebSocket Server Thread Function
unsigned WINAPI cWebServer::ThreadFunction(cWebServer* server)
{
	double mdt = 0.0;
	while (eState::Connect == server->m_state)
	{
		server->Process();

		//const int sleepTime = server->m_sessions.empty() ? 100 : server->m_sleepMillis;
		//if (sleepTime > 0)
		//	std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}
	return 0;
}
