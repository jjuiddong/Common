
#include "stdafx.h"
#include "udpclientmap.h"

using namespace network2;


cUdpClientMap::cUdpClientMap()
	: m_isThreadLoop(true)
	, m_packetSize(network2::DEFAULT_PACKETSIZE)
	, m_packetCount(network2::DEFAULT_PACKETCOUNT)
	, m_sleepMillis(network2::DEFAULT_SLEEPMILLIS)
	, m_isThreadMode(false)
	, m_logId(-1)
{
}

cUdpClientMap::~cUdpClientMap()
{
	Clear();
}


bool cUdpClientMap::Init(const int startUdpPort, const int portCount
	, const int packetSize //= network2::DEFAULT_PACKETSIZE
	, const int packetCount //= network2::DEFAULT_PACKETCOUNT
	, const int sleepMillis //= network2::DEFAULT_SLEEPMILLIS
	, const int logId //= -1
	, const bool isThread //= false;
)
{
	Clear();

	// prepare udp port
	for (int i = 0; i < portCount; ++i)
		m_portMap.push_back({ startUdpPort + i, false });

	m_packetSize = packetSize;
	m_packetCount = packetCount;
	m_sleepMillis = sleepMillis;
	m_logId = logId;
	m_isThreadMode = isThread;

	m_isThreadLoop = true;
	m_thread = std::thread(cUdpClientMap::ThreadFunction, this);

	return true;
}


// Add Udp Client and Start, return udp port
// return: udp port, if fail, return -1
int cUdpClientMap::AddUdpClient(const StrId &name, const Str16 &ip
	, const int udpPort //= -1
)
{
	sClientData &clientData = FindUdpClient(name);
	if (clientData.client)
		return clientData.client->m_port; // Already Exist 

	const int port = (udpPort < 0) ? GetReadyPort() : udpPort;
	if (port < 0)
		return -1;

	sThreadMsg msg;
	msg.type = sThreadMsg::START_CLIENT;
	msg.name = name;
	msg.ip = ip;
	msg.port = port;
	m_cs.Lock();
	m_sendThreadMsgs.push_back(msg);
	m_cs.Unlock();
	return port;
}


// Remove UDP Client
bool cUdpClientMap::RemoveUdpClient(const StrId &name)
{
	auto &udpSvr = FindUdpClient(name);
	if (!udpSvr.client)
		return false; // Not Exist

	sThreadMsg msg;
	msg.type = sThreadMsg::REMOVE_CLIENT;
	msg.name = name;
	m_cs.Lock();
	m_sendThreadMsgs.push_back(msg);
	m_cs.Unlock();

	return true;
}


cUdpClientMap::sClientData& cUdpClientMap::FindUdpClient(const StrId &name)
{
	static sClientData nullClientData{ "null", nullptr };
	AutoCSLock cs(m_cs);
	auto it = m_clients.find(name);
	if (m_clients.end() == it)
		return nullClientData;
	return it->second;
}


// return availible udp port
// if not exist, return -1
int cUdpClientMap::GetReadyPort()
{
	common::AutoCSLock cs(m_cs);
	for (auto &port : m_portMap)
	{
		if (!port.second)
		{
			port.second = true;
			return port.first;
		}
	}
	return -1;
}


// update availible port
void cUdpClientMap::SetReadyPort(const int readyPort)
{
	common::AutoCSLock cs(m_cs);
	for (auto &port : m_portMap)
	{
		if (readyPort == port.first)
		{
			port.second = false;
			break;
		}
	}
}


void cUdpClientMap::Clear()
{
	m_isThreadLoop = false;
	if (m_thread.joinable())
		m_thread.join();
	m_portMap.clear();

	for (auto &it : m_clients)
	{
		if (it.second.client)
		{
			it.second.client->Close();
			delete it.second.client;
		}
	}
	m_clients.clear();
	m_sendThreadMsgs.clear();
	m_recvThreadMsgs.clear();
}


// udpclientmap thread function
int cUdpClientMap::ThreadFunction(cUdpClientMap *udpCliMap)
{
	network2::cNetController netController;

	while (udpCliMap->m_isThreadLoop)
	{
		// Process Message
		while (udpCliMap->m_isThreadLoop && !udpCliMap->m_sendThreadMsgs.empty())
		{
			sThreadMsg msg;
			udpCliMap->m_cs.Lock();
			msg = udpCliMap->m_sendThreadMsgs.front();
			//common::rotatepopvector(udpCliMap->m_sendThreadMsgs, 0);
			common::removevector2(udpCliMap->m_sendThreadMsgs, 0);
			udpCliMap->m_cs.Unlock();

			switch (msg.type)
			{
			case sThreadMsg::START_CLIENT:
			{
				network2::cUdpClient *client = new network2::cUdpClient(
					StrId("UdpClient") + "-" + msg.name
					, udpCliMap->m_logId);

				netController.StartUdpClient(client, msg.ip, msg.port
					, udpCliMap->m_packetSize, udpCliMap->m_packetCount, udpCliMap->m_sleepMillis
					, udpCliMap->m_isThreadMode);

				udpCliMap->m_cs.Lock();
				sClientData clientData;
				clientData.name = msg.name;
				clientData.client = client;
				udpCliMap->m_clients.insert({ msg.name, clientData });
				udpCliMap->m_cs.Unlock();
			}
			break;

			case sThreadMsg::REMOVE_CLIENT:
			{
				udpCliMap->m_cs.Lock();
				auto it = udpCliMap->m_clients.find(msg.name);
				if (udpCliMap->m_clients.end() != it)
				{
					auto svr = it->second;
					netController.RemoveClient(svr.client);
					udpCliMap->SetReadyPort(svr.client->m_port);
					udpCliMap->m_clients.erase(msg.name);
					delete svr.client;
				}
				udpCliMap->m_cs.Unlock();
			}
			break;

			default: assert(0); break;
			}
		} // ~Process Message

		netController.Process(0.001f);

		if (!udpCliMap->m_isThreadMode && !udpCliMap->m_clients.empty())
		{
			udpCliMap->m_cs.Lock();
			for (auto kv : udpCliMap->m_clients)
				kv.second.client->Process();
			udpCliMap->m_cs.Unlock();
		}

		const int sleepTime = udpCliMap->m_clients.empty() ? 100 : udpCliMap->m_sleepMillis;
		if (sleepTime > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}

	return 0;
}
