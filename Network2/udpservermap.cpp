
#include "stdafx.h"
#include "udpservermap.h"

using namespace network2;


//----------------------------------------------------------------------------------
// cSpawnUdpServerTask
//class cSpawnUdpServerTask : public common::cTask
//{
//public:
//	cNetController *m_netController;
//	cUdpServerMap *m_udpSvrMap;
//	StrId m_keyName; // udpservermap key name
//	cUdpServer *m_svr;
//	int m_bindPort;
//
//	cSpawnUdpServerTask(cNetController *netController, cUdpServerMap *svrMap
//		, const StrId &keyName, cUdpServer *svr, const int bindPort)
//		: common::cTask(1, "cSpawnUdpServerTask")
//		, m_netController(netController)
//		, m_udpSvrMap(svrMap)
//		, m_keyName(keyName)
//		, m_svr(svr)
//		, m_bindPort(bindPort)
//	{
//	}
//
//	virtual eRunResult Run(const double deltaSeconds)
//	{
//		cUdpServerMap::sServerData *svrData = nullptr;
//		{
//			AutoCSLock cs(m_udpSvrMap->m_csSvr);
//			auto it = m_udpSvrMap->m_svrs.find(m_keyName);
//			if (m_udpSvrMap->m_svrs.end() != it)
//				svrData = &it->second;
//		}
//
//		if (!svrData)
//			return eRunResult::End; // not found, maybe deleted
//
//		// udpserver bind
//		const bool result = m_svr->Init(m_bindPort
//			, m_udpSvrMap->m_packetSize, m_udpSvrMap->m_packetCount
//			, m_udpSvrMap->m_sleepMillis, m_udpSvrMap->m_isThreadMode);
//		if (!result)
//		{
//			// udpserver initialize error!!
//			// send basic protocol, ErrorSession event trigger
//			m_svr->m_recvQueue.Push(m_svr->m_id, ErrorBindPacket(m_svr));
//			// send udpsvrmap protocol, Error event trigger
//			m_svr->m_recvQueue.Push(m_svr->m_id, udpsvrmap::SendError(m_svr, 0));
//		}
//
//		{
//			AutoCSLock cs(m_udpSvrMap->m_csSvr);
//			auto it = m_udpSvrMap->m_svrs.find(m_keyName);
//			if (m_udpSvrMap->m_svrs.end() != it)
//			{
//				it->second.svr = m_svr; // update instance ptr
//				m_netController->m_udpServers.push_back(m_svr);
//			}
//		}
//		return eRunResult::End;
//	}
//};


//----------------------------------------------------------------------------------
// cDeleteUdpServerTask
//class cDeletUdpServerTask : public common::cTask
//{
//public:
//	cUdpServer *m_svr;
//	cDeletUdpServerTask(cUdpServer *svr) : common::cTask(1, "cDeletUdpServerTask")
//		, m_svr(svr) { }
//	virtual eRunResult Run(const double deltaSeconds)
//	{
//		SAFE_DELETE(m_svr);
//		return eRunResult::End;
//	}
//};



//----------------------------------------------------------------------------------
// cUdpServerMap
cUdpServerMap::cUdpServerMap()
	: m_isLoop(true)
	, m_packetSize(network2::DEFAULT_PACKETSIZE)
	, m_packetCount(network2::DEFAULT_PACKETCOUNT)
	, m_sleepMillis(network2::DEFAULT_SLEEPMILLIS)
	, m_logId(-1)
	, m_isThreadMode(true)
	, m_portCursor(0)
{
}

cUdpServerMap::~cUdpServerMap()
{
	Clear();
}


// startUdpBindPort: udpserver bind starting port
// maxServerCount: maximum udpserver count
bool cUdpServerMap::Init(const int startUdpBindPort
	, const int maxServerCount
	, const int packetSize //= network2::DEFAULT_PACKETSIZE
	, const int packetCount //= network2::DEFAULT_PACKETCOUNT
	, const int sleepMillis //= network2::DEFAULT_SLEEPMILLIS
	, const int logId //= -1
	, const bool isThreadMode //= false
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
	const int bindPort = GetReadyPort();
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
	//AutoCSLock cs(m_csSvr);
	auto it = m_svrs.find(name);
	if (m_svrs.end() == it)
		return nullSvrData;
	return it->second;
}


// return availible port
// return -1, no availible port
int cUdpServerMap::GetReadyPort()
{
	//common::AutoCSLock cs(m_csSvr);

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
	//for (auto &info : m_bindPortMap)
	//{
	//	if (!info.used && (0 == info.error))
	//	{
	//		info.used = true;
	//		return info.port;
	//	}
	//}
	return -1;
}


// set unused port
void cUdpServerMap::SetReadyPort(const int port)
{
	//common::AutoCSLock cs(m_csSvr);
	auto it = find_if(m_bindPortMap.begin(), m_bindPortMap.end()
		, [&](auto &a) { return port == a.port; });
	if (m_bindPortMap.end() != it)
		it->used = false;
}


// set port error
void cUdpServerMap::SetPortError(const int port, const int error)
{
	//common::AutoCSLock cs(m_csSvr);
	auto it = find_if(m_bindPortMap.begin(), m_bindPortMap.end()
		, [&](auto &a) { return port == a.port; });
	if (m_bindPortMap.end() != it)
		it->error = error;
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

	while (!m_freeSvrs.empty())
	{
		delete m_freeSvrs.front();
		m_freeSvrs.pop();
	}
}


int cUdpServerMap::ThreadFunction(cUdpServerMap *udpSvrMap)
{
	network2::cNetController netController;

	cTimer timer;
	timer.Create();
	double incT = 0.0;
	int loop = 0;

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
				//udpSvrMap->m_csSvr.Lock();
				auto it = udpSvrMap->m_svrs.find(msg.name);
				isAlreadyExist = (udpSvrMap->m_svrs.end() != it);
				//udpSvrMap->m_csSvr.Unlock();

				if (isAlreadyExist)
				{
					if (msg.port != it->second.svr->m_port)
						udpSvrMap->SetReadyPort(msg.port);
					break; // already exist server, ignore
				}

				network2::cUdpServer *svr = nullptr;
				if (udpSvrMap->m_freeSvrs.empty())
				{
					svr = new network2::cUdpServer(StrId("UdpServer") + "-" + msg.name
						, udpSvrMap->m_logId);
					svr->AddProtocolHandler(msg.handler);
				}
				else
				{
					// reuse udpserver
					svr = udpSvrMap->m_freeSvrs.front();
					udpSvrMap->m_freeSvrs.pop();
					svr->m_name = StrId("UdpServer") + "-" + msg.name;
					svr->AddProtocolHandler(msg.handler);
				}

				//if (udpSvrMap->m_isThreadMode)
				//{
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

						udpSvrMap->SetPortError(msg.port, 1);
					}
				//}
				//else
				//{
				//	// no thread mode, but server binding is multithreading
				//	udpSvrMap->m_spawnThread.PushTask(
				//		new cSpawnUdpServerTask(&netController, udpSvrMap, msg.name
				//			, svr, msg.port));
				//}
				
				{
					//AutoCSLock cs(udpSvrMap->m_csSvr);
					sServerData svrData;
					svrData.name = msg.name;
					//svrData.svr = udpSvrMap->m_isThreadMode? svr : nullptr;
					svrData.svr = svr;
					udpSvrMap->m_svrs.insert({ msg.name, svrData });
				}
			}
			break;

			case sThreadMsg::eType::RemoveServer:
			{
				//udpSvrMap->m_csSvr.Lock();
				auto it = udpSvrMap->m_svrs.find(msg.name);
				int svrPort = -1;
				if (udpSvrMap->m_svrs.end() != it)
				{
					auto info = it->second;
					netController.RemoveServer(info.svr);
					if (info.svr)
					{
						svrPort = info.svr->m_port;

						// tricky code, udpserver close
						info.svr->m_state = cSession::eState::Disconnect;
						if (info.svr->m_thread.joinable())
							info.svr->m_thread.join();
						info.svr->cNetworkNode::Close(); // only socket close
					}
					udpSvrMap->m_svrs.erase(msg.name);

					// send udpsvrmap protocol, Close event trigger
					// to remove instance, send immediate
					info.svr->m_recvQueue.ClearBuffer();
					info.svr->m_recvQueue.Push(info.svr->m_id, udpsvrmap::SendClose(info.svr));
					netController.Dispatch(info.svr);

					if (info.svr)
					{
						// initialize udpserver to reuse
						info.svr->m_protocolHandlers.clear();
						info.svr->m_recvQueue.ClearBuffer();
						udpSvrMap->m_freeSvrs.push(info.svr);
					}
				}
				//udpSvrMap->m_csSvr.Unlock();
				if (svrPort > 0) // outside cs, to avoid deadlock
					udpSvrMap->SetReadyPort(svrPort);
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
			//AutoCSLock cs(udpSvrMap->m_csSvr);
			for (auto &kv : udpSvrMap->m_svrs)
				if (kv.second.svr && kv.second.svr->IsConnect())
					kv.second.svr->Process();

			netController.Process(0.001f);
		}

		const double dt = timer.GetDeltaSeconds();
		incT += dt;
		++loop;
		if (incT > 1.0)
		{
			incT = 0.0;
			//dbg::Logc(1, "UdpServerMap Loop: %d\n", loop);
			loop = 0;
		}

		const int sleepTime = udpSvrMap->m_svrs.empty() ? 100 : udpSvrMap->m_sleepMillis;
		if (sleepTime > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	}

	//udpSvrMap->m_spawnThread.Clear();
	return 0;
}
