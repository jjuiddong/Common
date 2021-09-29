
#include "stdafx.h"
#include "udpservermap.h"

using namespace network2;


//----------------------------------------------------------------------------------
// cSpawnUdpServerTask
class cSpawnUdpServerTask : public common::cTask
{
public:
	cNetController *m_netController;
	cUdpServerMap *m_udpSvrMap;
	StrId m_keyName; // udpservermap key name
	cUdpServer *m_svr;
	int m_bindPort;

	cSpawnUdpServerTask(cNetController *netController, cUdpServerMap *svrMap
		, const StrId &keyName, cUdpServer *svr, const int bindPort)
		: common::cTask(1, "cSpawnUdpServerTask")
		, m_netController(netController)
		, m_udpSvrMap(svrMap)
		, m_keyName(keyName)
		, m_svr(svr)
		, m_bindPort(bindPort)
	{
	}

	virtual eRunResult Run(const double deltaSeconds)
	{
		cUdpServerMap::sServerData *svrData = nullptr;
		{
			AutoCSLock cs(m_udpSvrMap->m_csSvr);
			auto it = m_udpSvrMap->m_svrs.find(m_keyName);
			if (m_udpSvrMap->m_svrs.end() != it)
				svrData = &it->second;
		}

		if (!svrData)
		{
			// update port availible
			m_udpSvrMap->SetReadyBindPort(m_bindPort);
			delete m_svr;
			return eRunResult::End; // not found
		}

		// udpserver bind
		const bool result = m_svr->Init(m_bindPort
			, m_udpSvrMap->m_packetSize, m_udpSvrMap->m_packetCount
			, m_udpSvrMap->m_sleepMillis, m_udpSvrMap->m_isThreadMode);
		if (!result)
		{
			// udpserver initialize error!!
			// send basic protocol, ErrorSession event trigger
			m_svr->m_recvQueue.Push(m_svr->m_id, ErrorBindPacket(m_svr));
			// send udpsvrmap protocol, Error event trigger
			m_svr->m_recvQueue.Push(m_svr->m_id, udpsvrmap::SendError(m_svr, 0));
		}

		{
			AutoCSLock cs(m_udpSvrMap->m_csSvr);
			auto it = m_udpSvrMap->m_svrs.find(m_keyName);
			if (m_udpSvrMap->m_svrs.end() != it)
			{
				it->second.svr = m_svr; // update instance ptr
				m_netController->m_udpServers.push_back(m_svr);
			}
		}
		return eRunResult::End;
	}
};


//----------------------------------------------------------------------------------
// cDeleteUdpServerTask
class cDeletUdpServerTask : public common::cTask
{
public:
	cUdpServer *m_svr;
	cDeletUdpServerTask(cUdpServer *svr) : common::cTask(1, "cDeletUdpServerTask")
		, m_svr(svr) { }
	virtual eRunResult Run(const double deltaSeconds)
	{
		SAFE_DELETE(m_svr);
		return eRunResult::End;
	}
};



//----------------------------------------------------------------------------------
// cUdpServerMap
cUdpServerMap::cUdpServerMap()
	: m_isLoop(true)
	, m_packetSize(network2::DEFAULT_PACKETSIZE)
	, m_packetCount(network2::DEFAULT_PACKETCOUNT)
	, m_sleepMillis(network2::DEFAULT_SLEEPMILLIS)
	, m_logId(-1)
	, m_isThreadMode(true)
{
}

cUdpServerMap::~cUdpServerMap()
{
	Clear();
}


bool cUdpServerMap::Init(const int startUdpBindPort, const int bindCount
	, const int packetSize //= network2::DEFAULT_PACKETSIZE
	, const int packetCount //= network2::DEFAULT_PACKETCOUNT
	, const int sleepMillis //= network2::DEFAULT_SLEEPMILLIS
	, const int logId //= -1
	, const bool isThreadMode //= false
)
{
	// update port mapper
	m_bindPortMap.reserve(bindCount);
	for (int i = 0; i < bindCount; ++i)
		m_bindPortMap.push_back({ startUdpBindPort + i, false });

	m_packetSize = packetSize;
	m_packetCount = packetCount;
	m_sleepMillis = sleepMillis;
	m_logId = logId;
	m_isThreadMode = isThreadMode;

	m_isLoop = true;
	m_thread = std::thread(cUdpServerMap::ThreadFunction, this);
	return true;
}


// Add Udp Server and Start, return udp bind port
// if fail, return -1
int cUdpServerMap::AddUdpServer(const StrId &name
	, network2::iProtocolHandler *handler)
{
	const int bindPort = GetReadyBindPort();
	if (bindPort < 0)
		return -1;

	sThreadMsg msg;
	msg.type = sThreadMsg::eType::StartServer;
	msg.name = name;
	msg.port = bindPort;
	msg.handler = handler;
	m_csMsg.Lock();
		m_sendThreadMsgs.push_back(msg);
	m_csMsg.Unlock();
	return bindPort;
}


// Remove UDP Server
bool cUdpServerMap::RemoveUdpServer(const StrId &name)
{
	sThreadMsg msg;
	msg.type = sThreadMsg::eType::RemoveServer;
	msg.name = name;
	m_csMsg.Lock();
		m_sendThreadMsgs.push_back(msg);
	m_csMsg.Unlock();
	return true;
}


cUdpServerMap::sServerData& cUdpServerMap::FindUdpServer(const StrId &name)
{
	static sServerData nullSvrData{ "null", nullptr };
	AutoCSLock cs(m_csSvr);
	auto it = m_svrs.find(name);
	if (m_svrs.end() == it)
		return nullSvrData;
	return it->second;
}


// return availible port
// -1: no port
int cUdpServerMap::GetReadyBindPort()
{
	common::AutoCSLock cs(m_csSvr);
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


// set unused port
void cUdpServerMap::SetReadyBindPort(const int bindPort)
{
	common::AutoCSLock cs(m_csSvr);
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
}


int cUdpServerMap::ThreadFunction(cUdpServerMap *udpSvrMap)
{
	network2::cNetController netController;

	while (udpSvrMap->m_isLoop)
	{
		// Process Message
		int procCnt = 0; // message process count
		while (udpSvrMap->m_isLoop 
			&& (procCnt++ < 5)
			&& !udpSvrMap->m_sendThreadMsgs.empty())
		{
			sThreadMsg msg;
			udpSvrMap->m_csMsg.Lock();
			msg = udpSvrMap->m_sendThreadMsgs.front();
			common::rotatepopvector(udpSvrMap->m_sendThreadMsgs, 0);
			udpSvrMap->m_csMsg.Unlock();

			switch (msg.type)
			{
			case sThreadMsg::eType::StartServer:
			{
				bool isAlreadyExist = false;
				udpSvrMap->m_csSvr.Lock();
				auto it = udpSvrMap->m_svrs.find(msg.name);
				isAlreadyExist = (udpSvrMap->m_svrs.end() != it);
				udpSvrMap->m_csSvr.Unlock();

				if (isAlreadyExist)
				{
					if (msg.port != it->second.svr->m_port)
						udpSvrMap->SetReadyBindPort(msg.port);
					break; // already exist server, ignore
				}

				network2::cUdpServer *svr = new network2::cUdpServer(
					StrId("UdpServer") + "-" + msg.name
					, udpSvrMap->m_logId);
				svr->AddProtocolHandler(msg.handler);

				if (udpSvrMap->m_isThreadMode)
				{
					const bool result = netController.StartUdpServer(svr, msg.port
						, udpSvrMap->m_packetSize, udpSvrMap->m_packetCount
						, udpSvrMap->m_sleepMillis, udpSvrMap->m_isThreadMode);
					if (!result)
					{
						// udpserver initialize error!!
						// send basic protocol, ErrorSession event trigger
						svr->m_recvQueue.Push(svr->m_id, ErrorBindPacket(svr));
						// send udpsvrmap protocol, Error event trigger
						svr->m_recvQueue.Push(svr->m_id, udpsvrmap::SendError(svr, 0));
					}
				}
				else
				{
					// no thread mode, but server binding is multithreading
					udpSvrMap->m_spawnThread.PushTask(
						new cSpawnUdpServerTask(&netController, udpSvrMap, msg.name
							, svr, msg.port));
				}
				
				{
					AutoCSLock cs(udpSvrMap->m_csSvr);
					sServerData svrData;
					svrData.name = msg.name;
					svrData.svr = udpSvrMap->m_isThreadMode? svr : nullptr;
					udpSvrMap->m_svrs.insert({ msg.name, svrData });
				}
			}
			break;

			case sThreadMsg::eType::RemoveServer:
			{
				udpSvrMap->m_csSvr.Lock();
				auto it = udpSvrMap->m_svrs.find(msg.name);
				int svrPort = -1;
				if (udpSvrMap->m_svrs.end() != it)
				{
					auto info = it->second;
					netController.RemoveServer(info.svr);
					if (info.svr)
					{
						svrPort = info.svr->m_port;
						info.svr->Close();
					}
					udpSvrMap->m_svrs.erase(msg.name);

					// send udpsvrmap protocol, Close event trigger
					// to remove instance, send immediate
					info.svr->m_recvQueue.ClearBuffer();
					info.svr->m_recvQueue.Push(info.svr->m_id, udpsvrmap::SendClose(info.svr));
					netController.Dispatch(info.svr);

					// delete multithread
					udpSvrMap->m_spawnThread.PushTask(new cDeletUdpServerTask(info.svr));
				}
				udpSvrMap->m_csSvr.Unlock();
				if (svrPort > 0) // outside cs, to avoid deadlock
					udpSvrMap->SetReadyBindPort(svrPort);
			}
			break;

			default: assert(0); break;
			}
		} // ~Process Message

		if (udpSvrMap->m_isThreadMode)
		{
			netController.Process(0.001f);
		}
		else
		{
			AutoCSLock cs(udpSvrMap->m_csSvr);
			for (auto &kv : udpSvrMap->m_svrs)
				if (kv.second.svr && kv.second.svr->IsConnect())
					kv.second.svr->Process();

			netController.Process(0.001f);
		}

		const int sleepTime = udpSvrMap->m_svrs.empty() ? 100 : udpSvrMap->m_sleepMillis;
		if (sleepTime > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}

	udpSvrMap->m_spawnThread.Clear();
	return 0;
}


//----------------------------------------------------------------------------------
// cUdpServerMap Basic Protocol
using namespace udpsvrmap;
cPacketHeader udpsvrmap::Dispatcher::s_packetHeader;
Dispatcher::Dispatcher()
	: cProtocolDispatcher(udpsvrmap::dispatcher_ID, ePacketFormat::BINARY)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({ dispatcher_ID, this });
	cProtocolDispatcher::GetPacketHeaderMap()->insert({ dispatcher_ID, &s_packetHeader });
}

bool udpsvrmap::Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case PACKETID_UDPSVRMAP_CLOSE:
	{
		ProtocolHandlers prtHandler;
		if (!HandlerMatching<ProtocolHandler>(handlers, prtHandler))
			return false;
		SetCurrentDispatchPacket(&packet);
		Close_Packet data;
		data.pdispatcher = this;
		data.senderId = packet.GetSenderId();
		packet >> data.serverName;
		SEND_HANDLER(ProtocolHandler, prtHandler, Close(data));
	}
	break;

	case PACKETID_UDPSVRMAP_ERROR:
	{
		ProtocolHandlers prtHandler;
		if (!HandlerMatching<ProtocolHandler>(handlers, prtHandler))
			return false;
		SetCurrentDispatchPacket(&packet);
		Error_Packet data;
		data.pdispatcher = this;
		data.senderId = packet.GetSenderId();
		packet >> data.serverName;
		packet >> data.errCode;
		SEND_HANDLER(ProtocolHandler, prtHandler, Error(data));
	}
	break;
	default:
		assert(0);
		break;
	}
	return true;
}


// return reserved Close packet
cPacket udpsvrmap::SendClose(network2::cNetworkNode *node)
{
	cPacket packet(GetPacketHeader(ePacketFormat::BINARY));
	packet.SetSenderId(node->m_id);
	packet.SetProtocolId(dispatcher_ID);
	packet.SetPacketId(PACKETID_UDPSVRMAP_CLOSE);
	packet << node->m_name;
	packet.EndPack();
	return packet;
}


// return reserved Error packet
cPacket udpsvrmap::SendError(network2::cNetworkNode *node, int errCode)
{
	cPacket packet(GetPacketHeader(ePacketFormat::BINARY));
	packet.SetSenderId(node->m_id);
	packet.SetProtocolId(dispatcher_ID);
	packet.SetPacketId(PACKETID_UDPSVRMAP_ERROR);
	packet << node->m_name;
	packet << errCode;
	packet.EndPack();
	return packet;
}
