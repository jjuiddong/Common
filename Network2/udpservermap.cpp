
#include "stdafx.h"
#include "udpservermap.h"

using namespace network2;

cUdpServerMap::cUdpServerMap()
	: m_isThreadLoop(true)
	, m_packetSize(network2::DEFAULT_PACKETSIZE)
	, m_packetCount(network2::DEFAULT_MAX_PACKETCOUNT)
	, m_sleepMillis(network2::DEFAULT_SLEEPMILLIS)
	, m_logId(-1)
{
}

cUdpServerMap::~cUdpServerMap()
{
	Clear();
}


bool cUdpServerMap::Init(const int startUdpBindPort, const int bindCount
	, const int packetSize //= network2::DEFAULT_PACKETSIZE
	, const int packetCount //= network2::DEFAULT_MAX_PACKETCOUNT
	, const int sleepMillis //= network2::DEFAULT_SLEEPMILLIS
	, const int logId //= -1
)
{
	// 미리 UDP Server Bind Port를 등록한다.
	for (int i = 0; i < bindCount; ++i)
		m_bindPortMap.push_back({ startUdpBindPort + i, false });

	m_packetSize = packetSize;
	m_packetCount = packetCount;
	m_sleepMillis = sleepMillis;
	m_logId = logId;

	m_isThreadLoop = true;
	m_thread = std::thread(cUdpServerMap::ThreadFunction, this);

	return true;
}


// Add Udp Server and Start, return udp bind port
// if fail, return -1
int cUdpServerMap::AddUdpServer(const StrId &name
	, network2::iProtocolHandler *handler)
{
	sServerData &svrData = FindUdpServer(name);
	if (svrData.svr)
		return svrData.svr->m_port; // Already Exist 

	const int bindPort = GetReadyBindPort();
	if (bindPort < 0)
		return -1;

	sThreadMsg msg;
	msg.type = sThreadMsg::START_SERVER;
	msg.name = name;
	msg.port = bindPort;
	msg.handler = handler;
	m_cs.Lock();
		m_sendThreadMsgs.push_back(msg);
	m_cs.Unlock();
	return bindPort;
}


// Remove UDP Server
bool cUdpServerMap::RemoveUdpServer(const StrId &name)
{
	auto &udpSvr = FindUdpServer(name);
	if (!udpSvr.svr)
		return false; // Not Exist

	sThreadMsg msg;
	msg.type = sThreadMsg::REMOVE_SERVER;
	msg.name = name;
	m_cs.Lock();
		m_sendThreadMsgs.push_back(msg);
	m_cs.Unlock();

	return true;
}


cUdpServerMap::sServerData& cUdpServerMap::FindUdpServer(const StrId &name)
{
	static sServerData nullSvrData{ "null", NULL };
	AutoCSLock cs(m_cs);
	auto it = m_svrs.find(name);
	if (m_svrs.end() == it)
		return nullSvrData;
	return it->second;
}


// 쓰지않는 udp server bind port를 리턴한다.
// 없으면 -1
int cUdpServerMap::GetReadyBindPort()
{
	common::AutoCSLock cs(m_cs);
	for (auto &port : m_bindPortMap)
	{
		if (!port.second)
		{
			port.second = true;
			return port.first;
		}
	}
	return -1;
}


// port를 bind할 수 있는 포트로 설정한다.
void cUdpServerMap::SetReadyBindPort(const int bindPort)
{
	common::AutoCSLock cs(m_cs);
	for (auto &port : m_bindPortMap)
	{
		if (bindPort == port.first)
		{
			port.second = false;
			break;
		}
	}
}


void cUdpServerMap::Clear()
{
	m_isThreadLoop = false;
	if (m_thread.joinable())
		m_thread.join();

	for (auto &it : m_svrs)
	{
		if (it.second.svr)
		{
			it.second.svr->Close();
			delete it.second.svr;
		}
	}
	m_svrs.clear();
}


int cUdpServerMap::ThreadFunction(cUdpServerMap *udpSvrMap)
{
	network2::cNetController netController;

	while (udpSvrMap->m_isThreadLoop)
	{
		// Process Message
		while (udpSvrMap->m_isThreadLoop && !udpSvrMap->m_sendThreadMsgs.empty())
		{
			sThreadMsg msg;
			udpSvrMap->m_cs.Lock();
			msg = udpSvrMap->m_sendThreadMsgs.front();
			common::rotatepopvector(udpSvrMap->m_sendThreadMsgs, 0);
			udpSvrMap->m_cs.Unlock();

			switch (msg.type)
			{
			case sThreadMsg::START_SERVER:
			{
				network2::cUdpServer *svr = new network2::cUdpServer(
					StrId("UdpServer") + "-" + msg.name
					, udpSvrMap->m_logId);
				svr->AddProtocolHandler(msg.handler);

				netController.StartUdpServer(svr, msg.port
					, udpSvrMap->m_packetSize, udpSvrMap->m_packetCount, udpSvrMap->m_sleepMillis);

				udpSvrMap->m_cs.Lock();
				sServerData svrData;
				svrData.name = msg.name;
				svrData.svr = svr;
				udpSvrMap->m_svrs.insert({ msg.name, svrData });
				udpSvrMap->m_cs.Unlock();
			}
			break;

			case sThreadMsg::REMOVE_SERVER:
			{
				udpSvrMap->m_cs.Lock();
				auto it = udpSvrMap->m_svrs.find(msg.name);
				if (udpSvrMap->m_svrs.end() != it)
				{
					auto svr = it->second;
					netController.RemoveServer(svr.svr);
					udpSvrMap->SetReadyBindPort(svr.svr->m_port);
					udpSvrMap->m_svrs.erase(msg.name);
					//udpSvrMap->m_ids.erase(svr.svrId);
					delete svr.svr;
				}
				udpSvrMap->m_cs.Unlock();
			}
			break;

			default: assert(0); break;
			}
		} // ~Process Message

		netController.Process(0.001f);

		const int sleepTime = udpSvrMap->m_svrs.empty() ? 100 : udpSvrMap->m_sleepMillis;
		if (sleepTime > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}

	return 0;
}
