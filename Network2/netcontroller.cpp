
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
	, const int maxPacketCount //= DEFAULT_MAX_PACKETCOUNT
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
	, const int maxPacketCount //= DEFAULT_MAX_PACKETCOUNT
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
	, const int maxPacketCount //= DEFAULT_MAX_PACKETCOUNT
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
	, const int maxPacketCount //= DEFAULT_MAX_PACKETCOUNT
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


bool cNetController::StartWebClient(cWebClient *client
	, const string &url
	, const int port
	, const int packetSize //= DEFAULT_PACKETSIZE
	, const int maxPacketCount //= DEFAULT_MAX_PACKETCOUNT
	, const int sleepMillis //= DEFAULT_SLEEPMILLIS
	, const bool isThread //= true
)
{
	const bool result = client->Init(url, port, packetSize
		, maxPacketCount, sleepMillis, isThread);

	if (!IsExistClient(client))
		m_webClients.push_back(client);

	return true;
}


template<class NetNode, class Dispatcher>
int ProcessNetworkNode(NetNode *netNode, Dispatcher *basicDispatcher)
{
	int procPacketCnt = 0;
	
	cPacket packet(netNode->GetPacketHeader());

	while (1000 > procPacketCnt)
	{
		packet.Initialize();
		if (!netNode->m_recvQueue.Front(packet))
			break;

		++procPacketCnt;

		if (!packet.IsValid())
			continue;

		auto& handlers = netNode->GetProtocolHandlers();
		const int protocolId = packet.GetProtocolId();
		const uint packetId = packet.GetPacketId();

		// Process Basic Protocol
		if ((protocolId == 0) || (packetId < 10))
		{
			basicDispatcher->Dispatch(packet, netNode);
			continue;
		}

		// All Protocol Dispatcher
		all::Dispatcher allDispatcher;
		allDispatcher.Dispatch(packet, handlers);
		//

		if (handlers.empty())
		{
			dbg::Logc(2, " Error!! cNetController::Process() 프로토콜 핸들러가 없습니다. protocol: %d, netid: %d\n"
				, protocolId, netNode->GetId());
			continue;
		}

		auto it = cProtocolDispatcher::GetDispatcherMap()->find(packet.GetProtocolId());
		if (cProtocolDispatcher::GetDispatcherMap()->end() == it)
		{
			dbg::Logc(2, " Error!! cNetController::Process() %d 에 해당하는 프로토콜 디스패쳐가 없습니다.\n",
				packet.GetPacketId());
		}
		else
		{
			it->second->Dispatch(packet, handlers);
		}
	}

	return procPacketCnt;
}


// 등록된 서버와 클라이언트에서 받은 패킷을 핸들러에게 전달한다.
// 처리한 패킷 개수를 리턴한다.
int cNetController::Process(const float deltaSeconds)
{
	int procPacketCnt = 0;

	// Server Process
	basic_protocol::ServerDispatcher svrDispatcher;
	for (auto &p : m_tcpServers)
		procPacketCnt += ProcessNetworkNode(p, &svrDispatcher);

	basic_protocol::UdpServerDispatcher udpSvrDispatcher;
	for (auto &p : m_udpServers)
		procPacketCnt += ProcessNetworkNode(p, &udpSvrDispatcher);

	// Client Process
	basic_protocol::ClientDispatcher clientDispatcher;
	for (auto &p : m_tcpClients)
		procPacketCnt += ProcessNetworkNode(p, &clientDispatcher);

	basic_protocol::WebClientDispatcher webClientDispatcher;
	for (auto &p : m_webClients)
		procPacketCnt += ProcessNetworkNode(p, &webClientDispatcher);

	return procPacketCnt;
}


bool cNetController::RemoveServer(cNetworkNode *svr)
{
	if (!IsExistServer(svr))
		return false;

	if (cTcpServer *p = (cTcpServer*)dynamic_cast<cTcpServer*>(svr))
		common::removevector(m_tcpServers, p);
	if (cUdpServer *p = (cUdpServer*)dynamic_cast<cUdpServer*>(svr))
		common::removevector(m_udpServers, p);

	return true;
}


bool cNetController::RemoveClient(cNetworkNode *svr)
{
	if (!IsExistClient(svr))
		return false;

	if (cTcpClient *p = (cTcpClient*)dynamic_cast<cTcpClient*>(svr))
		common::removevector(m_tcpClients, p);
	if (cUdpClient *p = (cUdpClient*)dynamic_cast<cUdpClient*>(svr))
		common::removevector(m_udpClients, p);
	if (cWebClient *p = (cWebClient*)dynamic_cast<cWebClient*>(svr))
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
