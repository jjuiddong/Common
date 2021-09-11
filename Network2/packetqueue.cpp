#include "stdafx.h"
#include "packetqueue.h"
#include "session.h"

using namespace network2;


cPacketQueue::cPacketQueue(cNetworkNode *node
	, const int logId //= -1
)
	: m_logId(logId)
	, m_isLogIgnorePacket(true)
	, m_netNode(node)
	, m_nextFrontIdx(0)
	, m_sockBufferSize(0)
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

	m_sockBufferSize = packetSize * maxPacketCount;
	return true;
}


// push packet
bool cPacketQueue::Push(const netid rcvId, const cPacket &packet)
{
	return Push(rcvId, packet.m_header, packet.m_data, packet.GetPacketSize());
}


// push packet data
bool cPacketQueue::Push(const netid senderId, const BYTE *data, const int len)
{
	return Push(senderId, nullptr, data, len);
}


// push packet
bool cPacketQueue::Push(const netid senderId, iPacketHeader *packetHeader
	, const BYTE *data, const int len)
{
	cAutoCS cs(m_cs);

	cSocketBuffer *sockBuff = NULL;
	auto it = m_sockBuffers.find(senderId);
	if (m_sockBuffers.end() == it)
	{
		sockBuff = new cSocketBuffer(senderId, m_sockBufferSize);
		m_sockBuffers.insert({ senderId, sockBuff });
	}
	else
	{
		sockBuff = it->second;
	}

	const uint addLen = sockBuff->Push(packetHeader, data, len);
	if (0 == addLen)
	{
		if (m_isLogIgnorePacket)
			common::dbg::ErrLog("packetqueue Push alloc error!! \n");
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
	
	// write packet log?
	if ((m_logId >= 0) && result)
		network2::LogPacket(m_logId, out.GetSenderId(), m_netNode->m_id, out);

	return result;
}


void cPacketQueue::SendAll(
	const map<netid, SOCKET> &socks
	, OUT set<netid> *outErrSocks //= NULL
)
{
	RET(m_sockBuffers.empty());

	cAutoCS cs(m_cs);

	bool isSendError = false;
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
			cPacket packet;
			if (!sockBuffer->PopNoRemove(packet))
				break;

			if (ALL_NETID == sockBuffer->m_netId)
			{
				const bool result = SendBroadcast(socks, packet, outErrSocks);
				if (!result)
				{
					// error!!, no remove buffer, resend
					dbg::Logc(2, "Error Send Packet\n");
					isSendError = true;
					break;
				}

				// write packet log?
				if (m_logId >= 0)
					network2::LogPacket(m_logId, m_netNode->m_id, 0, packet);
			}
			else
			{
				int result = 0;
				if (INVALID_SOCKET != sock)
				{
					const int result = m_netNode->SendPacket(sock, packet);
					if (result != packet.GetPacketSize())
					{
						// error!!, no remove buffer, resend
						dbg::Logc(2, "Error Send Packet\n");
						isSendError = true;
						break;
					}

					// write packet log?
					if ((m_logId >= 0) && (result != SOCKET_ERROR))
						network2::LogPacket(m_logId, m_netNode->m_id
							, sockBuffer->m_netId, packet);
				}

				if (outErrSocks && (result == SOCKET_ERROR))
				{
					// error!!, close socket
					outErrSocks->insert(sockBuffer->m_netId);
				}
			} //~else ALL_NETID

			sockBuffer->Pop(packet.m_writeIdx);

		} // ~while
	} // ~for

	if (isSendError)
	{
		// send 에러가 발생하면, 딜레이를 줘서 다른 쓰레드로 스위칭할 수 있게한다.
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


// send all
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
			cPacket packet;
			if (!sockBuffer->Pop(packet))
				break;

			if (INVALID_SOCKET == sock)
			{
				assert(0);
			}
			else
			{
				m_netNode->SendToPacket(sock, sockAddr, packet);
			}
		}
	}
}


// exceptOwner 가 true 일때, 패킷을 보낸 클라이언트를 제외한 나머지 클라이언트들에게 모두
// 패킷을 보낸다.
bool cPacketQueue::SendBroadcast(
	const vector<cSession*> &sessions
	, const bool exceptOwner // =true
)
{
	cAutoCS cs(m_cs);

	for (uint i = 0; i < m_sockBuffers.m_seq.size(); ++i)
	{
		cSocketBuffer *sockBuffer = m_sockBuffers.m_seq[i];
		while (1)
		{
			cPacket packet;
			if (!sockBuffer->Pop(packet))
				break;

			for (u_int k = 0; k < sessions.size(); ++k)
			{
				// exceptOwner가 true일 때, 패킷을 준 클라이언트에게는 보내지 않는다.
				const bool isSend = !exceptOwner 
					|| (exceptOwner && (sockBuffer->m_netId != sessions[k]->m_id));
				if (isSend)
					m_netNode->SendPacket(sessions[k]->m_socket, packet);
			}
		}
	}

	return true;
}


// return false if send error
bool cPacketQueue::SendBroadcast(const map<netid, SOCKET> &socks
	, const cPacket &packet
	, OUT set<netid> *outErrSocks //= NULL
)
{
	bool isSendError = false;
	for (auto &kv : socks)
	{
		if (kv.first == SERVER_NETID)
			continue; // broadcast server -> client

		const SOCKET sock = kv.second;
		const int result = m_netNode->SendPacket(sock, packet);

		if (result == SOCKET_ERROR)
		{
			isSendError = true;
			if (outErrSocks)
				outErrSocks->insert(kv.first);
		}
	}
	return !isSendError;
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
