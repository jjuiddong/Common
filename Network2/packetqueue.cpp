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
	ClearBuffer();

	if (0 == m_sockBufferSize)
		m_sockBufferSize = packetSize * maxPacketCount;
	else if (m_sockBufferSize != (packetSize * maxPacketCount))
		assert(0);
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

	cSocketBuffer *sockBuff = nullptr;
	auto it = m_sockBuffers.find(senderId);
	if (m_sockBuffers.end() == it)
	{
		if (!m_frees.empty())
		{
			sockBuff = *m_frees.begin();
			m_frees.erase(*m_frees.begin());
			sockBuff->m_netId = senderId;
		}
		else
		{
			sockBuff = new cSocketBuffer(senderId, m_sockBufferSize);
		}
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


// return front of queue element
bool cPacketQueue::Front(OUT cPacket &out)
{
	RETV(m_sockBuffers.empty(), false);

	cAutoCS cs(m_cs);

	if (m_nextFrontIdx >= (int)m_sockBuffers.m_seq.size())
		m_nextFrontIdx = 0;

	bool result = false;
	uint cnt = 0;
	int idx = m_nextFrontIdx;
	while (cnt++ < m_sockBuffers.size())
	{
		cSocketBuffer *sockBuff = m_sockBuffers.m_seq[idx++];
		idx %= (int)m_sockBuffers.size();
		if (sockBuff->IsEmpty())
			continue;

		out.m_sndId = sockBuff->m_netId;
		result = sockBuff->Pop(out);
	
		// write packet log?
		if ((m_logId >= 0) && result)
			network2::LogPacket(m_logId, out.GetSenderId(), m_netNode->m_id, out);
		break;
	}

	m_nextFrontIdx = idx;
	return result;
}


// send packet all
void cPacketQueue::SendAll(
	const map<netid, SOCKET> &socks
	, OUT set<netid> *outErrs //= nullptr
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
				const bool result = SendBroadcast(socks, packet, outErrs);
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
					result = m_netNode->SendPacket(sock, packet);
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

				if (outErrs && (result == SOCKET_ERROR))
				{
					// error!!, close socket
					outErrs->insert(sockBuffer->m_netId);
				}
			} //~else ALL_NETID

			sockBuffer->Pop(packet.m_writeIdx);

		} // ~while
	} // ~for

	if (isSendError)
	{
		// when occur error, sleep some seconds to thread context switching
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


// send packet all 
void cPacketQueue::SendAll(
	set<netid> *outErrs //= nullptr
)
{
	RET(m_sockBuffers.empty());

	cAutoCS cs(m_cs);

	bool isSendError = false;
	for (uint i = 0; i < m_sockBuffers.m_seq.size(); ++i)
	{
		cSocketBuffer *sockBuffer = m_sockBuffers.m_seq[i];
		if (sockBuffer->IsEmpty())
			continue;
		while (1)
		{
			cPacket packet;
			if (!sockBuffer->PopNoRemove(packet))
				break;

			if (ALL_NETID == sockBuffer->m_netId)
			{
				m_netNode->SendAll(packet, outErrs);
			}
			else
			{
				int result = m_netNode->SendImmediate(sockBuffer->m_netId, packet);
				if (result != packet.GetPacketSize())
				{
					// error!!, no remove buffer, resend
					dbg::Logc(2, "Error Send Packet\n");
					result = SOCKET_ERROR;
					isSendError = true;
				}

				// write packet log?
				if ((m_logId >= 0) && (result != SOCKET_ERROR))
					network2::LogPacket(m_logId, m_netNode->m_id
						, sockBuffer->m_netId, packet);

				if (outErrs && (result == SOCKET_ERROR))
				{
					// error!!, close socket
					outErrs->insert(sockBuffer->m_netId);
				}
			} //~else ALL_NETID

			sockBuffer->Pop(packet.m_writeIdx);
		} // ~while
	} // ~for

	if (isSendError)
	{
		// when occur error, sleep some seconds to thread context switching
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


// send packet all, udp
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


// send packet to sessions
// exceptOwner: if true except self
bool cPacketQueue::SendBroadcast(const vector<cSession*> &sessions
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
				// send self?
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
	, OUT set<netid> *outErrSocks //= nullptr
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


// remove socket buffer correspond netid
bool cPacketQueue::Remove(const netid id)
{
	cAutoCS cs(m_cs);

	auto it = m_sockBuffers.find(id);
	if (m_sockBuffers.end() == it)
		return false; // not found
	it->second->Clear();
	m_frees.insert(it->second);
	m_sockBuffers.remove(id);
	return true;
}


// update logid
void cPacketQueue::SetLogId(const int logId)
{
	m_logId = logId;
}


void cPacketQueue::Lock()
{
	EnterCriticalSection(&m_cs);
}


void cPacketQueue::Unlock()
{
	LeaveCriticalSection(&m_cs);
}


// clear buffer
void cPacketQueue::ClearBuffer()
{
	cAutoCS cs(m_cs);
	for (auto &buff : m_sockBuffers.m_seq)
	{
		buff->Clear();
		m_frees.insert(buff);
	}
	m_sockBuffers.clear();
	m_nextFrontIdx = 0;
}


void cPacketQueue::Clear()
{
	cAutoCS cs(m_cs);
	for (auto sock : m_sockBuffers.m_seq)
		delete sock;
	m_sockBuffers.clear();
	for (auto &sock : m_frees)
		delete sock;
	m_frees.clear();
	m_nextFrontIdx = 0;
}
