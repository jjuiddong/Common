#include "stdafx.h"
#include "packetqueue.h"
#include "session.h"

using namespace network2;


cPacketQueue::cPacketQueue(cNetworkNode *node
	, const bool isPacketLog //= false
)
	: m_isPacketLog(isPacketLog)
	, m_isLogIgnorePacket(false)
	, m_netNode(node)
	, m_nextFrontIdx(0)
{
	InitializeCriticalSectionAndSpinCount(&m_cs, 0x00000400);
}

cPacketQueue::~cPacketQueue()
{
	Clear();

	DeleteCriticalSection(&m_cs);
}


bool cPacketQueue::Init(const int packetSize, const int maxPacketCount)
{
	Clear();

	return true;
}


// 패킷을 큐에 저장한다.
// data는 정확히 패킷 크기만큼의 정보를 가져야한다.
// 쪼개진 패킷을 합치는 코드는 없다.
// 네트워크로 부터 받아서 저장하는게 아니라, 사용자가 직접 패킷을 큐에 넣을 때
// 사용하는 함수다.
// return true : 성공
//		  false : 실패
bool cPacketQueue::Push(const netid rcvId, const cPacket &packet)
{
	cAutoCS cs(m_cs);

	cSocketBuffer *sockBuff = NULL;
	auto it = m_sockBuffers.find(rcvId);
	if (m_sockBuffers.end() == it)
	{
		sockBuff = new cSocketBuffer(rcvId);
		m_sockBuffers.insert({ rcvId, sockBuff });
	}
	else
	{
		sockBuff = it->second;
	}

	const uint addLen = sockBuff->Push(packet.m_packetHeader, packet.m_data, packet.GetPacketSize());
	if (0 == addLen)
	{
		if (m_isLogIgnorePacket)
			common::dbg::ErrLog("packetqueue push Alloc error!! \n");
	}
	return 0 != addLen;
}


// 네트워크로 부터 온 패킷일 경우,
// 여러개로 나눠진 패킷을 처리할 때도 호출할 수 있다.
// *data가 두개 이상의 패킷을 포함할 때도, 이를 처리한다.
// senderId : 패킷을 송신한 유저의 network id
// return true : 성공
//		  false : 실패
bool cPacketQueue::PushFromNetwork(const netid senderId, const BYTE *data, const int len)
{
	cAutoCS cs(m_cs);

	cSocketBuffer *sockBuff = NULL;
	auto it = m_sockBuffers.find(senderId);
	if (m_sockBuffers.end() == it)
	{
		sockBuff = new cSocketBuffer(senderId);
		m_sockBuffers.insert({ senderId, sockBuff });
	}
	else
	{
		sockBuff = it->second;
	}

	const uint addLen = sockBuff->Push(m_netNode->GetPacketHeader(), data, len);
	if (0 == addLen)
	{
		if (m_isLogIgnorePacket)
			common::dbg::ErrLog("packetqueue PushFromNetwork Alloc error!! \n");
	}

	return true;
}


bool cPacketQueue::Front(OUT cPacket &out)
{
	RETV(m_sockBuffers.empty(), false);

	cAutoCS cs(m_cs);

	if (m_nextFrontIdx >= (int)m_sockBuffers.m_seq.size())
		m_nextFrontIdx = 0;
	
	cSocketBuffer *sockBuff = m_sockBuffers.m_seq[m_nextFrontIdx++];
	out.m_sndId = sockBuff->m_netId;
	const bool result = sockBuff->Pop(out);
	
	// write packet log
	if (m_isPacketLog && result)
		network2::LogPacket(m_netNode->m_id, out);

	return result;
}


void cPacketQueue::SendAll(
	const map<netid, SOCKET> &socks
	, OUT set<netid> *outErrSocks //= NULL
)
{
	RET(m_sockBuffers.empty());

	cAutoCS cs(m_cs);

	for (u_int i = 0; i < m_sockBuffers.m_seq.size(); ++i)
	{
		cSocketBuffer *sockBuffer = m_sockBuffers.m_seq[i];

		SOCKET sock = INVALID_SOCKET;
		if (ALL_NETID != sockBuffer->m_netId)
		{
			auto it = socks.find(sockBuffer->m_netId);
			if (it == socks.end())
			{
				// Already Close Socket, no problem
			}
			else
			{
				sock = it->second;
			}
		}

		while (1)
		{
			cPacket packet(m_netNode->GetPacketHeader());
			if (!sockBuffer->Pop(packet))
				break;

			if (ALL_NETID == sockBuffer->m_netId)
			{
				SendBroadcast(socks, packet, outErrSocks);

				// write packet log
				if (m_isPacketLog)
					network2::LogPacket(m_netNode->m_id, packet);
			}
			else
			{
				int result = 0;
				if (INVALID_SOCKET != sock)
				{
					result = send(sock, (const char*)packet.m_data, packet.GetPacketSize(), 0);

					// write packet log
					if (m_isPacketLog && (result != SOCKET_ERROR))
						network2::LogPacket(m_netNode->m_id, packet);
				}

				if (outErrSocks && (result == SOCKET_ERROR))
					outErrSocks->insert(sockBuffer->m_netId);
			}

		} // ~while
	} // ~for
}


// udpclient send all
void cPacketQueue::SendAll(const sockaddr_in &sockAddr)
{
	RET(m_sockBuffers.empty());

	cAutoCS cs(m_cs);
	for (u_int i = 0; i < m_sockBuffers.m_seq.size(); ++i)
	{
		cSocketBuffer *sockBuffer = m_sockBuffers.m_seq[i];
		const SOCKET sock = m_netNode->GetSocket(sockBuffer->m_netId);
		while (1)
		{
			cPacket packet(m_netNode->GetPacketHeader());
			if (!sockBuffer->Pop(packet))
				break;

			if (INVALID_SOCKET == sock)
			{
				assert(0);
			}
			else
			{
				sendto(sock, (const char*)packet.m_data, packet.GetPacketSize()
					, 0, (struct sockaddr*) &sockAddr, sizeof(sockAddr));
			}
		}
	}
}


// exceptOwner 가 true 일때, 패킷을 보낸 클라이언트를 제외한 나머지 클라이언트들에게 모두
// 패킷을 보낸다.
void cPacketQueue::SendBroadcast(const vector<cSession*> &sessions, const bool exceptOwner)
{
	cAutoCS cs(m_cs);

	for (uint i = 0; i < m_sockBuffers.m_seq.size(); ++i)
	{
		cSocketBuffer *sockBuffer = m_sockBuffers.m_seq[i];
		while (1)
		{
			cPacket packet(m_netNode->GetPacketHeader());
			if (!sockBuffer->Pop(packet))
				break;

			for (u_int k = 0; k < sessions.size(); ++k)
			{
				// exceptOwner가 true일 때, 패킷을 준 클라이언트에게는 보내지 않는다.
				const bool isSend = !exceptOwner 
					|| (exceptOwner && (sockBuffer->m_netId != sessions[k]->m_id));
				if (isSend)
					send(sessions[k]->m_socket, (const char*)packet.m_data, packet.GetPacketSize(), 0);
			}
		}
	}
}


void cPacketQueue::SendBroadcast(const map<netid, SOCKET> &socks
	, const cPacket &packet
	, OUT set<netid> *outErrSocks //= NULL
)
{
	for (auto &kv : socks)
	{
		if (kv.first == SERVER_NETID)
			continue;

		const SOCKET sock = kv.second;
		const int result = send(sock, (const char*)packet.m_data, packet.GetPacketSize(), 0);

		if (outErrSocks && (result == SOCKET_ERROR))
			outErrSocks->insert(kv.first);
	}
}


void cPacketQueue::Lock()
{
	EnterCriticalSection(&m_cs);
}


void cPacketQueue::Unlock()
{
	LeaveCriticalSection(&m_cs);
}


void cPacketQueue::Clear()
{
	cAutoCS cs(m_cs);
	for (auto sock : m_sockBuffers.m_seq)
		delete sock;
	m_sockBuffers.clear();
	m_nextFrontIdx = 0;
}
