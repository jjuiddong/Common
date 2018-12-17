#include "stdafx.h"
#include "packetqueue.h"
#include "session.h"

using namespace network;


cPacketQueue::cPacketQueue()
	: m_memPoolPtr(NULL)
	, m_chunkBytes(0)
	, m_packetBytes(0)
	, m_totalChunkCount(0)
	, m_tempHeaderBuffer(NULL)
	, m_tempBuffer(NULL)
	, m_isStoreTempHeaderBuffer(false)
	, m_isLogIgnorePacket(false)
{
	InitializeCriticalSectionAndSpinCount(&m_criticalSection, 0x00000400);
}

cPacketQueue::~cPacketQueue()
{
	Clear();

	DeleteCriticalSection(&m_criticalSection);
}


bool cPacketQueue::Init(const int packetSize, const int maxPacketCount)
{
	Clear();

	// Init Memory pool
	m_packetBytes = packetSize + sizeof(sHeader);
	m_chunkBytes = packetSize;
	m_totalChunkCount = maxPacketCount;
	//m_memPoolPtr = new BYTE[(packetSize+sizeof(sHeader)) * maxPacketCount];
	m_memPool.reserve(maxPacketCount);
	for (int i = 0; i < maxPacketCount; ++i)
	{
		m_memPool.push_back({ false, new BYTE[ (packetSize + sizeof(sHeader)) ] });
		//m_memPool.push_back({ false, m_memPoolPtr + (i*(packetSize + sizeof(sHeader))) });
	}
	//

	m_queue.reserve(maxPacketCount);
	m_tempBuffer = new BYTE[packetSize + sizeof(sHeader)];
	m_tempBufferSize = packetSize + sizeof(sHeader);
	m_tempHeaderBuffer = new BYTE[sizeof(sHeader)];
	m_tempHeaderBufferSize = 0;

	return true;
}


// 패킷을 큐에 저장한다.
// data는 정확히 패킷 크기만큼의 정보를 가져야한다.
// 쪼개진 패킷을 합치는 코드는 없다.
// 네트워크로 부터 받아서 저장하는게 아니라, 사용자가 직접 패킷을 큐에 넣을 때
// 사용하는 함수다.
void cPacketQueue::Push(const SOCKET sock, const char protocol[4]
	, const BYTE *data, const int len)
{
	cAutoCS cs(m_criticalSection);

	int totalLen = len;
	int offset = 0;
	while (offset < totalLen)
	{
		const BYTE *ptr = data + offset;
		const int size = totalLen - offset;
		if (sSockBuffer *buffer = FindSockBuffer(sock))
		{
			offset += CopySockBuffer(buffer, ptr, size);
		}
		else
		{
			int addLen = AddSockBuffer(sock, protocol, ptr, size);
			if (0 == addLen)
			{
				if (m_isLogIgnorePacket)
					common::dbg::ErrLog("packetqueue push Alloc error!! \n");
				break;
			}

			offset += addLen;
		}
	}
}


// 네트워크로 부터 온 패킷일 경우,
// 여러개로 나눠진 패킷을 처리할 때도 호출할 수 있다.
// *data가 두개 이상의 패킷을 포함할 때도, 이를 처리한다.
void cPacketQueue::PushFromNetwork(const SOCKET sock, const BYTE *data, const int len)
{
	cAutoCS cs(m_criticalSection);

	int totalLen = len;

	// 남은 여분의 버퍼가 있을 때.. 
	// 임시로 저장해뒀던 정보와 인자로 넘어온 정보를 합쳐서 처리한다.
	if (m_isStoreTempHeaderBuffer)
	{
		if (m_tempBufferSize < (len + m_tempHeaderBufferSize))
		{
			SAFE_DELETEA(m_tempBuffer);
			m_tempBuffer = new BYTE[len + m_tempHeaderBufferSize];
			m_tempBufferSize = len + m_tempHeaderBufferSize;
		}

		memcpy(m_tempBuffer, m_tempHeaderBuffer, m_tempHeaderBufferSize);
		memcpy(m_tempBuffer + m_tempHeaderBufferSize, data, len);	
		m_isStoreTempHeaderBuffer = false;

		data = m_tempBuffer;
		totalLen = m_tempHeaderBufferSize + len;
	}

	int offset = 0;
	while (offset < totalLen)
	{
		const BYTE *ptr = data + offset;
		const int size = totalLen - offset;
		if (sSockBuffer *buffer = FindSockBuffer(sock))
		{
			offset += CopySockBuffer(buffer, ptr, size);
		}
		else
		{
			// 남은 여분의 데이타가 sHeader 보다 작을 때, 임시로 
			// 저장한 후, 나머지 패킷이 왔을 때 처리한다.
			if (size < sizeof(sHeader))
			{
				m_tempHeaderBufferSize = size;
				m_isStoreTempHeaderBuffer = true;
				memcpy(m_tempHeaderBuffer, ptr, size);
				offset += size;
			}
			else
			{
				int addLen = AddSockBufferByNetwork(sock, ptr, size);
				if (0 == addLen)
				{
					if (m_isLogIgnorePacket)
						common::dbg::ErrLog("packetqueue push Alloc error!! \n");
					break;
				}

				offset += addLen;
			}
		}
	}
}


// 크리티컬섹션이 먼저 생성된 후에 호출하자.
// sock 에 해당하는 큐를 리턴한다. 
sSockBuffer* cPacketQueue::FindSockBuffer(const SOCKET sock)
{
	RETV(m_queue.empty(), NULL);

	// find specific packet by socket
	for (u_int i = 0; i < m_queue.size(); ++i)
	{
		if (sock != m_queue[i].sock)
			continue;
		// 해당 소켓이 채워지지 않은 버퍼라면, 이 버퍼에 채우게 한다.
		if (m_queue[i].readLen < m_queue[i].totalLen)
			return &m_queue[i];
	}

	// fastmode?
	// 최대 패킷 수를 1개만 생성한 경우, 버퍼 하나에 업데이트하는 방식으로 작동한다.
	if (m_totalChunkCount == 1)
	{
		m_queue.front().readLen = 0;
		return &m_queue.front();
	}

	return NULL;
}


// cSockBuffer *dst에 data 를 len 만큼 저장한다.
// 이 때, len이 SockBuffer의 버퍼크기 만큼 저장한다.
// 복사 된 크기를 바이트 단위로 리턴한다.
int cPacketQueue::CopySockBuffer(sSockBuffer *dst, const BYTE *data, const int len)
{
	RETV(!dst, 0);
	RETV(!dst->buffer, 0);

	const int copyLen = min(dst->totalLen - dst->readLen, len);
	memcpy(dst->buffer + dst->readLen, data, copyLen);
	dst->readLen += copyLen;

	if (dst->readLen == dst->totalLen)
		dst->full = true;

	return copyLen;
}


// 새 패킷 버퍼를 추가한다.
int cPacketQueue::AddSockBuffer(const SOCKET sock, const char protocol[4]
	, const BYTE *data, const int len)
{
	// 새로 추가될 소켓의 패킷이라면
	sSockBuffer sockBuffer;
	sockBuffer.protocol[0] = protocol[0];
	sockBuffer.protocol[1] = protocol[1];
	sockBuffer.protocol[2] = protocol[2];
	sockBuffer.protocol[3] = protocol[3];
	sockBuffer.sock = sock;
	sockBuffer.totalLen = 0;
	sockBuffer.readLen = 0;
	sockBuffer.full = false;

	// 송신할 패킷을 추가할 경우, or 헤더가 없는 패킷을 받을 경우.
	// 패킷 헤더를 추가한다.
	sockBuffer.totalLen = len + sizeof(sHeader);
	sockBuffer.actualLen = len;

	if ((sockBuffer.totalLen < 0) || (sockBuffer.actualLen < 0))
	{
		common::dbg::ErrLog("packetqueue push error!! canceled 2.\n");
		return len;
	}

	sockBuffer.buffer = Alloc();

	if (!sockBuffer.buffer)
		return 0; // Error!! 

	sHeader header = MakeHeader(protocol, len + sizeof(sHeader));
	CopySockBuffer(&sockBuffer, (BYTE*)&header, sizeof(sHeader));

	const int copyLen = CopySockBuffer(&sockBuffer, data, len);
	m_queue.push_back(sockBuffer);

	// 패킷헤더 크기는 제외(순수하게 data에서 복사된 크기를 리턴한다.)
	return copyLen;
}


// 새 패킷 버퍼를 추가한다.
int cPacketQueue::AddSockBufferByNetwork(const SOCKET sock, const BYTE *data, const int len)
{
	// 네트워크를 통해 온 패킷이라면, 이미 패킷헤더가 포함된 상태다.
	// 전체 패킷 크기를 저장해서, 분리된 패킷인지를 판단한다.
	int byteSize = 0;
	const sHeader header = GetHeader(data, byteSize);
	if (isalpha(header.protocol[0])
		&& isalpha(header.protocol[1])
		&& isalpha(header.protocol[2])
		&& isalpha(header.protocol[3])
		&& (byteSize > sizeof(sHeader)) && (byteSize <= m_packetBytes))
	{
		// nothing~ continue~
	}
	else
	{
		// error occur!!
		// 패킷의 시작부가 아닌데, 시작부로 들어왔음.
		// 헤더부가 깨졌거나, 기존 버퍼가 Pop 되었음.
		// 무시하고 종료한다.
		common::dbg::ErrLog("packetqueue push error!! packet header not found\n");
		return len;
	}

	// 헤더를 제외한 데이타크기
	const int actualDataSize = byteSize - sizeof(sHeader);

	// 네트워크로부터 받은 패킷은 헤더부분을 제외한 실제 데이타만 복사한다.
	sSockBuffer sockBuffer;
	sockBuffer.sock = sock;
	sockBuffer.protocol[0] = header.protocol[0];
	sockBuffer.protocol[1] = header.protocol[1];
	sockBuffer.protocol[2] = header.protocol[2];
	sockBuffer.protocol[3] = header.protocol[3];
	sockBuffer.readLen = 0;
	sockBuffer.totalLen = actualDataSize;
	sockBuffer.actualLen = actualDataSize; 
	sockBuffer.buffer = Alloc();
	sockBuffer.full = false;

	if (!sockBuffer.buffer)
		return 0; // Error!! 

	if ((sockBuffer.totalLen < 0) || (sockBuffer.actualLen < 0))
	{
		common::dbg::ErrLog("packetqueue push error!! canceled 2.\n");
		return len;
	}

	// 헤더 이후의 데이타를 복사한다.
	const int copyLen = CopySockBuffer(&sockBuffer, data + sizeof(sHeader)
		, min(len - (int)sizeof(sHeader), actualDataSize));
	m_queue.push_back(sockBuffer);

	// 패킷헤더 크기까지 포함.
	return copyLen + sizeof(sHeader);
}


// 헤더구조체를 생성한다.
cPacketQueue::sHeader cPacketQueue::MakeHeader(const char protocol[4], const int len)
{
	sHeader header;
	header.protocol[0] = protocol[0];
	header.protocol[1] = protocol[1];
	header.protocol[2] = protocol[2];
	header.protocol[3] = protocol[3];

	header.packetLength[0] = min(9, len / 1000) + '0';
	header.packetLength[1] = min(9, (len % 1000) / 100) + '0';
	header.packetLength[2] = min(9, (len % 100) / 10) + '0';
	header.packetLength[3] = min(9,  len % 10) + '0';

	return header;
}


// 바이트 스트림으로부터 헤더 구조체를 생성한다.
cPacketQueue::sHeader cPacketQueue::GetHeader(const BYTE *data, OUT int &byteSize)
{
	sHeader header = *(sHeader*)data;

	byteSize = (header.packetLength[0] - '0') * 1000
		+ (header.packetLength[1] - '0') * 100
		+ (header.packetLength[2] - '0') * 10
		+ (header.packetLength[3] - '0') * 1;

	return header;
}


bool cPacketQueue::Front(OUT sSockBuffer &out)
{
	RETV(m_queue.empty(), false);

	cAutoCS cs(m_criticalSection);
	RETV(!m_queue[0].full, false);

	out = m_queue[0];
	out.buffer = m_queue[0].buffer; // 네트워크로부터 받은 패킷이라면, 헤더부는 제외한 데이타부만 존재한다.

	return true;
}


void cPacketQueue::Pop()
{
	cAutoCS cs(m_criticalSection);
	RET(m_queue.empty());

	Free(m_queue.front().buffer);
	common::rotatepopvector(m_queue, 0);
}


void cPacketQueue::SendAll(
	OUT vector<SOCKET> *outErrSocks //= NULL
)
{
	RET(m_queue.empty());

	cAutoCS cs(m_criticalSection);
	for (u_int i = 0; i < m_queue.size(); ++i)
	{
		const int result = send(m_queue[i].sock, (const char*)m_queue[i].buffer, m_queue[i].totalLen, 0);
		Free(m_queue[i].buffer);

		if (outErrSocks && (result == SOCKET_ERROR))
			outErrSocks->push_back(m_queue[i].sock);
	}
	m_queue.clear();
}


void cPacketQueue::SendAll(const sockaddr_in &sockAddr)
{
	RET(m_queue.empty());

	cAutoCS cs(m_criticalSection);
	for (u_int i = 0; i < m_queue.size(); ++i)
	{
		sendto(m_queue[i].sock, (const char*)m_queue[i].buffer, m_queue[i].totalLen,
			0, (struct sockaddr*) &sockAddr, sizeof(sockAddr));
		Free(m_queue[i].buffer);
	}
	m_queue.clear();
}


// 큐에 저장된 버퍼의 총 크기 (바이트 단위)
int cPacketQueue::GetSize()
{
	RETV(m_queue.empty(), 0);

	int size = 0;
	cAutoCS cs(m_criticalSection);
	for (u_int i = 0; i < m_queue.size(); ++i)
		size += m_queue[i].totalLen;
	return size;
}


// exceptOwner 가 true 일때, 패킷을 보낸 클라이언트를 제외한 나머지 클라이언트들에게 모두
// 패킷을 보낸다.
void cPacketQueue::SendBroadcast(vector<sSession> &sessions, const bool exceptOwner)
{
	cAutoCS cs(m_criticalSection);

	for (u_int i = 0; i < m_queue.size(); ++i)
	{
		if (!m_queue[i].full)
			continue; // 다 채워지지 않은 패킷은 무시

		for (u_int k = 0; k < sessions.size(); ++k)
		{
			// exceptOwner가 true일 때, 패킷을 준 클라이언트에게는 보내지 않는다.
			const bool isSend = !exceptOwner || (exceptOwner && (m_queue[i].sock != sessions[k].socket));
			if (isSend)
				send(sessions[k].socket, (const char*)m_queue[i].buffer, m_queue[i].totalLen, 0);
		}
	}

	// 모두 전송한 후 큐를 비운다.
	for (u_int i = 0; i < m_queue.size(); ++i)
		Free(m_queue[i].buffer);
	m_queue.clear();

	ClearMemPool();
}


void cPacketQueue::Lock()
{
	EnterCriticalSection(&m_criticalSection);
}


void cPacketQueue::Unlock()
{
	LeaveCriticalSection(&m_criticalSection);
}


BYTE* cPacketQueue::Alloc()
{
	for (u_int i = 0; i < m_memPool.size(); ++i)
	{
		if (!m_memPool[i].used)
		{
			m_memPool[i].used = true;
			return m_memPool[i].p;
		}
	}
	return NULL;
}


void cPacketQueue::Free(BYTE*ptr)
{
	for (u_int i = 0; i < m_memPool.size(); ++i)
	{
		if (ptr == m_memPool[i].p)
		{
			m_memPool[i].used = false;
			break;
		}
	}
}


void cPacketQueue::Clear()
{
	SAFE_DELETEA(m_memPoolPtr);
	SAFE_DELETEA(m_tempBuffer);
	SAFE_DELETEA(m_tempHeaderBuffer);
	
	for (auto &pool : m_memPool)
	{
		SAFE_DELETEA(pool.p);
	}
	m_memPool.clear();


	m_queue.clear();
}


// 메모리 풀을 초기화해서, 어쩌다 생길지 모를 버그를 제거한다.
void cPacketQueue::ClearMemPool()
{
	for (u_int i = 0; i < m_memPool.size(); ++i)
		m_memPool[i].used = false;
}
