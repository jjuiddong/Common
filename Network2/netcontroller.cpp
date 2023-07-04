
#include "stdafx.h"
#include "netcontroller.h"

using namespace network2;


cNetController::cNetController()
{
}

cNetController::~cNetController()
{
	Clear();
}


bool cNetController::StartTcpServer(cTcpServer *svr
	, const int bindPort
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //=true
)
{
	const bool result = svr->Init(bindPort, packetSize, maxPacketCount
		, sleepMillis, isThreadMode);

	if (!IsExistServer(svr))
		m_tcpServers.push_back(svr);

	return result;
}


bool cNetController::StartTcpClient(cTcpClient *client
	, const Str16 &ip
	, const int port
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const int clientSidePort //= -1
	, const bool isThread //= true
)
{
	const bool result = client->Init(ip, port, packetSize
		, maxPacketCount, sleepMillis, clientSidePort, isThread);

	if (!IsExistClient(client))
		m_tcpClients.push_back(client);

	return result;
}


bool cNetController::StartUdpServer(cUdpServer *svr
	, const int bindPort
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //=true
)
{
	const bool result = svr->Init(bindPort, packetSize, maxPacketCount
		, sleepMillis, isThreadMode);

	if (!IsExistServer(svr))
		m_udpServers.push_back(svr);

	return result;
}


bool cNetController::StartUdpClient(cUdpClient *client
	, const Str16 &ip
	, const int port
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThread //= true
)
{
	const bool result = client->Init(ip, port, packetSize
		, maxPacketCount, sleepMillis, isThread);

	if (!IsExistClient(client))
		m_udpClients.push_back(client);

	return result;
}


bool cNetController::StartWebServer(cWebServer *svr
	, const int bindPort
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThreadMode //=true
	, const bool isSpawnHttpSvr //= true
)
{
	const bool result = svr->Init(bindPort, packetSize, maxPacketCount
		, sleepMillis, isThreadMode, isSpawnHttpSvr);

	if (!IsExistServer(svr))
		m_webServers.push_back(svr);

	return result;
}


bool cNetController::StartWebClient(cWebClient *client
	, const string &url
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThread //= true
)
{
	const bool result = client->Init(url, packetSize
		, maxPacketCount, sleepMillis, isThread);

	if (!IsExistClient(client))
		m_webClients.push_back(client);

	return result;
}


template<class NetNode, class Dispatcher>
int ProcessNetworkNode(NetNode *netNode, Dispatcher *basicDispatcher)
{
	all::Dispatcher allDispatcher;
	cPacket packet;
	int procPacketCnt = 0;

	while (1000 > procPacketCnt)
	{
		if (!netNode->m_recvQueue.Front(packet))
			break;
		++procPacketCnt;
		packet.InitRead();
		if (!packet.IsValid())
			continue;

		auto& handlers = netNode->GetProtocolHandlers();
		auto& listeners = netNode->GetProtocolListeners();
		const int protocolId = packet.GetProtocolId();
		const uint packetId = packet.GetPacketId();

		// Process Basic Protocol
		if ((0 == protocolId) || (10 > packetId))
		{
			if (basicDispatcher)
				basicDispatcher->Dispatch(packet, netNode);
			continue;
		}

		if (handlers.empty() && listeners.empty())
			continue; // no handler, nothing to do

		// All Protocol Dispatcher
		allDispatcher.Dispatch(packet, handlers);
		allDispatcher.Dispatch(packet, listeners);

		auto it = cProtocolDispatcher::GetDispatcherMap()->find(packet.GetProtocolId());
		if (cProtocolDispatcher::GetDispatcherMap()->end() != it)
		{
			it->second->Dispatch(packet, handlers);
		}
		else
		{
			// not found dispatcher, nothing to do
		}
	}

	return procPacketCnt;
}


// process network nodes
// dispatch receive packet to handler
// return process packet count
int cNetController::Process(const float deltaSeconds)
{
	int cnt = 0;

	// Server Process
	basic_protocol::ServerDispatcher svrDispatcher;
	for (uint i=0; i < m_tcpServers.size(); ++i)
		cnt += ProcessNetworkNode(m_tcpServers[i], &svrDispatcher);

	basic_protocol::UdpServerDispatcher udpSvrDispatcher;
	for (uint i=0; i < m_udpServers.size(); ++i)
		cnt += ProcessNetworkNode(m_udpServers[i], &udpSvrDispatcher);

	basic_protocol::WebServerDispatcher webSvrDispatcher;
	for (uint i = 0; i < m_webServers.size(); ++i)
		cnt += ProcessNetworkNode(m_webServers[i], &webSvrDispatcher);

	// Client Process
	basic_protocol::ClientDispatcher clientDispatcher;
	for (uint i=0; i < m_tcpClients.size(); ++i)
		cnt += ProcessNetworkNode(m_tcpClients[i], &clientDispatcher);

	basic_protocol::WebClientDispatcher webClientDispatcher;
	for (uint i=0; i < m_webClients.size(); ++i)
		cnt += ProcessNetworkNode(m_webClients[i], &webClientDispatcher);

	return cnt;
}


// dispatch packet specific network node
int cNetController::Dispatch(cNetworkNode *node)
{
	if (cTcpServer *svr = dynamic_cast<cTcpServer*>(node))
	{
		basic_protocol::ServerDispatcher svrDispatcher;
		return ProcessNetworkNode(svr, &svrDispatcher);
	}
	else if (cUdpServer *svr = dynamic_cast<cUdpServer*>(node))
	{
		basic_protocol::UdpServerDispatcher udpSvrDispatcher;
		return ProcessNetworkNode(svr, &udpSvrDispatcher);
	}
	else if (cWebServer *svr = dynamic_cast<cWebServer*>(node))
	{
		basic_protocol::WebServerDispatcher webSvrDispatcher;
		return ProcessNetworkNode(svr, &webSvrDispatcher);
	}
	else if (cTcpClient *cli = dynamic_cast<cTcpClient*>(node))
	{
		basic_protocol::ClientDispatcher clientDispatcher;
		return ProcessNetworkNode(cli, &clientDispatcher);
	}
	else if (cWebClient *cli = dynamic_cast<cWebClient*>(node))
	{
		basic_protocol::WebClientDispatcher webClientDispatcher;
		return ProcessNetworkNode(cli, &webClientDispatcher);
	}
	return 0;
}


bool cNetController::RemoveServer(cNetworkNode *svr)
{
	if (!IsExistServer(svr))
		return false;

	if (cTcpServer *p = dynamic_cast<cTcpServer*>(svr))
		common::removevector(m_tcpServers, p);
	if (cUdpServer *p = dynamic_cast<cUdpServer*>(svr))
		common::removevector(m_udpServers, p);
	if (cWebServer *p = dynamic_cast<cWebServer*>(svr))
		common::removevector(m_webServers, p);

	return true;
}


bool cNetController::RemoveClient(cNetworkNode *cli)
{
	if (!IsExistClient(cli))
		return false;

	if (cTcpClient *p = dynamic_cast<cTcpClient*>(cli))
		common::removevector(m_tcpClients, p);
	if (cUdpClient *p = dynamic_cast<cUdpClient*>(cli))
		common::removevector(m_udpClients, p);
	if (cWebClient *p = dynamic_cast<cWebClient*>(cli))
		common::removevector(m_webClients, p);

	return true;
}


bool cNetController::IsExistServer(const cNetworkNode *svr)
{
	for (auto &p : m_tcpServers)
		if (p == svr)
			return true;

	for (auto &p : m_udpServers)
		if (p == svr)
			return true;

	for (auto &p : m_webServers)
		if (p == svr)
			return true;

	return false;
}


bool cNetController::IsExistClient(const cNetworkNode *cli)
{
	for (auto &p : m_tcpClients)
		if (p == cli)
			return true;

	for (auto &p : m_udpClients)
		if (p == cli)
			return true;

	for (auto &p : m_webClients)
		if (p == cli)
			return true;

	return false;
}


void cNetController::Clear()
{
	m_tcpServers.clear();
	m_tcpClients.clear();
	m_udpServers.clear();
	m_udpClients.clear();
	m_webClients.clear();
}
