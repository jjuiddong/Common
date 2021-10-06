
#include "stdafx.h"
#include "udpservermap2.h"

using namespace network2;


cUdpServerMap2::cUdpServerMap2()
	: m_isLoop(true)
	, m_packetSize(network2::DEFAULT_PACKETSIZE)
	, m_packetCount(network2::DEFAULT_PACKETCOUNT)
	, m_sleepMillis(network2::DEFAULT_SLEEPMILLIS)
	, m_logId(-1)
	, m_portCursor(0)
	, m_tempRecvBuffer(nullptr)
{
}

cUdpServerMap2::~cUdpServerMap2()
{
	Clear();
}


// startUdpBindPort: udpserver bind starting port
// maxServerCount: maximum udpserver count
bool cUdpServerMap2::Init(const int startUdpBindPort
	, const int maxServerCount
	, const int packetSize //= network2::DEFAULT_PACKETSIZE
	, const int packetCount //= network2::DEFAULT_PACKETCOUNT
	, const int sleepMillis //= network2::DEFAULT_SLEEPMILLIS
	, const int logId //= -1
)
{
	// update port mapper
	m_bindPortMap.reserve(maxServerCount);
	for (int i = 0; i < maxServerCount; ++i)
		m_bindPortMap.push_back({ startUdpBindPort + i, false, 0 });

	m_packetSize = packetSize;
	m_packetCount = packetCount;
	m_sleepMillis = sleepMillis;
	m_logId = logId;

	m_isLoop = true;
	m_thread = std::thread(cUdpServerMap2::ThreadFunction, this);
	return true;
}


// Add Udp Server and Start, return udp bind port
// if fail, return -1
int cUdpServerMap2::AddUdpServer(const StrId &name
	, network2::iProtocolHandler *handler)
{
	const int bindPort = GetReadyPort();
	if (bindPort < 0)
		return -1;

	sThreadMsg msg;
	msg.type = sThreadMsg::StartServer;
	msg.name = name;
	msg.port = bindPort;
	msg.handler = handler;
	m_csMsg.Lock();
		m_sendThreadMsgs.push_back(msg);
	m_csMsg.Unlock();
	return bindPort;
}


// Remove UDP Server
bool cUdpServerMap2::RemoveUdpServer(const StrId &name)
{
	sThreadMsg msg;
	msg.type = sThreadMsg::RemoveServer;
	msg.name = name;
	m_csMsg.Lock();
		m_sendThreadMsgs.push_back(msg);
	m_csMsg.Unlock();
	return true;
}


cUdpServerMap2::sServerData& cUdpServerMap2::FindUdpServer(const StrId &name)
{
	static sServerData nullSvrData{ "null", nullptr };
	auto it = m_svrs.find(name);
	if (m_svrs.end() == it)
		return nullSvrData;
	return it->second;
}


// return availible port
// return -1, no availible port
int cUdpServerMap2::GetReadyPort()
{
	uint cnt = 0;
	uint i = m_portCursor;
	while (cnt++ < m_bindPortMap.size())
	{
		i %= m_bindPortMap.size();
		sBindPort &info = m_bindPortMap[i];
		if (!info.used && (0 == info.error))
		{
			m_portCursor = i + 1;
			info.used = true;
			return info.port;
		}
		++i;
	}
	return -1;
}


// set unused port
void cUdpServerMap2::SetReadyPort(const int port)
{
	auto it = find_if(m_bindPortMap.begin(), m_bindPortMap.end()
		, [&](auto &a) { return port == a.port; });
	if (m_bindPortMap.end() != it)
		it->used = false;
}


// set port error
void cUdpServerMap2::SetPortError(const int port, const int error)
{
	auto it = find_if(m_bindPortMap.begin(), m_bindPortMap.end()
		, [&](auto &a) { return port == a.port; });
	if (m_bindPortMap.end() != it)
		it->error = error;
}


// message process
bool cUdpServerMap2::MessageProcess()
{
	int procCnt = 0; // message process count
	while (m_isLoop
		&& (procCnt++ < 5)
		&& !m_sendThreadMsgs.empty())
	{
		sThreadMsg msg;
		m_csMsg.Lock();
		msg = m_sendThreadMsgs.front();
		common::removevector2(m_sendThreadMsgs, 0);
		m_csMsg.Unlock();

		switch (msg.type)
		{
		case sThreadMsg::StartServer:
		{
			bool isAlreadyExist = false;
			auto it = m_svrs.find(msg.name);
			isAlreadyExist = (m_svrs.end() != it);

			if (isAlreadyExist)
			{
				if (msg.port != it->second.svr->m_port)
					SetReadyPort(msg.port);
				break; // already exist server, ignore
			}

			network2::cUdpServer *svr = nullptr;
			if (m_freeSvrs.empty())
			{
				svr = new network2::cUdpServer(StrId("UdpServer") + "-" + msg.name
					, m_logId);
				svr->AddProtocolHandler(msg.handler);
			}
			else
			{
				// reuse udpserver
				svr = m_freeSvrs.front();
				m_freeSvrs.pop();
				svr->m_name = StrId("UdpServer") + "-" + msg.name;
				svr->AddProtocolHandler(msg.handler);
			}

			const bool result = svr->Init(msg.port
				, m_packetSize, m_packetCount
				, m_sleepMillis, false);
			if (!result)
			{
				// udpserver initialize error!!
				// send basic protocol, ErrorSession event trigger
				svr->m_recvQueue.Push(svr->m_id, ErrorBindPacket(svr));
				// send udpsvrmap protocol, Error event trigger
				svr->m_recvQueue.Push(svr->m_id, udpsvrmap::SendError(svr, 0));

				SetPortError(msg.port, 1);
			}

			{
				sServerData svrData;
				svrData.name = msg.name;
				svrData.svr = svr;
				m_svrs.insert({ msg.name, svrData });
				if (result)
				{
					m_svrs2.insert({ svr->m_socket, svr });
					FD_SET(svr->m_socket, &m_readSockets);
				}
			}
		}
		break;

		case sThreadMsg::RemoveServer:
		{
			auto it = m_svrs.find(msg.name);
			int svrPort = -1;
			SOCKET socket = INVALID_SOCKET;
			if (m_svrs.end() != it)
			{
				auto info = it->second;
				if (info.svr)
				{
					socket = info.svr->m_socket;
					svrPort = info.svr->m_port;
					info.svr->Close();
				}
				m_svrs.erase(msg.name);
				m_svrs2.erase(socket);

				// send udpsvrmap protocol, Close event trigger
				// to remove instance, send immediate
				info.svr->m_recvQueue.ClearBuffer();
				info.svr->m_recvQueue.Push(info.svr->m_id, udpsvrmap::SendClose(info.svr));
				cNetController::Dispatch(info.svr);

				if (info.svr)
				{
					// initialize udpserver to reuse
					info.svr->m_protocolHandlers.clear();
					info.svr->m_recvQueue.ClearBuffer();
					m_freeSvrs.push(info.svr);
					FD_CLR(socket, &m_readSockets);
				}
			}
			if (svrPort > 0) // outside cs, to avoid deadlock
				SetReadyPort(svrPort);
		}
		break;

		default: assert(0); break;
		}
	}
	return true;
}


// packet recv process
bool cUdpServerMap2::ReceiveProcess()
{
	if (0 == m_readSockets.fd_count)
		return true;

	if (!m_tempRecvBuffer)
		m_tempRecvBuffer = new char[m_packetSize];

	const timeval t = { 0, 0 };
	const fd_set &sockets = m_readSockets;
	fd_set readSockets = m_readSockets;

	const int ret = select(readSockets.fd_count, &readSockets, nullptr, nullptr, &t);
	if (0 == ret)
		return true; // nothing
	if (SOCKET_ERROR == ret)
		return false; // error occur, nothing

	for (u_int i = 0; i < sockets.fd_count; ++i)
	{
		const SOCKET sock = sockets.fd_array[i];
		if (!FD_ISSET(sock, &readSockets))
			continue;

		const int result = recv(sock, m_tempRecvBuffer, m_packetSize, 0);
		auto it = m_svrs2.find(sock);
		if (m_svrs2.end() == it)
			continue; // not found udpserver

		cUdpServer* svr = it->second;
		if (SOCKET_ERROR == result || 0 == result)
		{
			// disconnect state
		}
		else
		{
			svr->m_recvQueue.Push(svr->m_id, (BYTE*)m_tempRecvBuffer, result);
		}
	}

	return true;
}


// clear instance
void cUdpServerMap2::Clear()
{
	m_isLoop = false;
	if (m_thread.joinable())
		m_thread.join();

	for (auto &it : m_svrs)
	{
		if (it.second.svr)
		{
			it.second.svr->Close();
			SAFE_DELETE(it.second.svr);
		}
	}
	m_svrs.clear();
	m_svrs2.clear();

	while (!m_freeSvrs.empty())
	{
		delete m_freeSvrs.front();
		m_freeSvrs.pop();
	}

	FD_ZERO(&m_readSockets);
	SAFE_DELETEA(m_tempRecvBuffer);
}


int cUdpServerMap2::ThreadFunction(cUdpServerMap2 *udpSvrMap)
{
	cTimer timer;
	timer.Create();
	//double incT = 0.0;
	//int loop = 0;

	while (udpSvrMap->m_isLoop)
	{
		udpSvrMap->MessageProcess();
		udpSvrMap->ReceiveProcess();

		auto it = udpSvrMap->m_svrs2.begin();
		while (it != udpSvrMap->m_svrs2.end())
		{
			cUdpServer *svr = it++->second;
			cNetController::Dispatch(svr);
		}

		const int sleepTime = udpSvrMap->m_svrs.empty() ? 100 : udpSvrMap->m_sleepMillis;
		if (sleepTime > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}

	return 0;
}
