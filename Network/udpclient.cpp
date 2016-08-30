
#include "stdafx.h"
#include "udpclient.h"

using namespace std;
using namespace network;

unsigned WINAPI UDPClientThreadFunction(void* arg);


cUDPClient::cUDPClient() : 
	m_isConnect(false)
	, m_threadLoop(true)
//	, m_sndBuffLen(0)
//	, m_isSendData(false)
	, m_sleepMillis(30)
	, m_maxBuffLen(BUFFER_LENGTH)
{
//	m_sndBuffer = new BYTE[m_maxBuffLen];
}

cUDPClient::~cUDPClient()
{
	Close();

//	delete[] m_sndBuffer;
}


// UDP 클라언트 생성, ip, port 에 접속을 시도한다.
bool cUDPClient::Init(const string &ip, const int port, const int sleepMillis) //sleepMillis=30
{
	Close();

	m_ip = ip;
	m_port = port;
	m_sleepMillis = sleepMillis;

	if (network::LaunchUDPClient(ip, port, m_sockaddr, m_socket))
	{
		dbg::Log("Connect UDP Client ip=%s, port=%d\n", ip.c_str(), port);

		if (!m_sndQueue.Init(m_maxBuffLen, 512))
		{
			Close();
			return false;
		}

		m_threadLoop = false;
		if (m_thread.joinable())
			m_thread.join();

		m_isConnect = true;
		m_threadLoop = true;
		m_thread = std::thread(UDPClientThreadFunction, this);
	}
	else
	{
		dbg::Log("Error!! Connect UDP Client ip=%s, port=%d\n", ip.c_str(), port);
		return false;
	}
	
	return true;
}


// 전송할 정보를 설정한다.
void cUDPClient::SendData(const BYTE *buff, const int buffLen)
{
	m_sndQueue.Push(m_socket, buff, buffLen);

// 	AutoCSLock cs(m_sndCriticalSection);
// 	m_sndBuffLen = min(buffLen, m_maxBuffLen);
// 	memcpy(m_sndBuffer, buff, m_sndBuffLen);
// 	m_isSendData = true;
}


void cUDPClient::SetMaxBufferLength(const int length)
{
	m_maxBuffLen = length;

// 	if (m_maxBuffLen != length)
// 	{
// 		delete[] m_sndBuffer;
// 
// 		m_maxBuffLen = length;
// 		m_sndBuffer = new BYTE[length];
// 	}
}


// 접속을 끊는다.
void cUDPClient::Close()
{
	m_isConnect = false;
	m_threadLoop = false;
	if (m_thread.joinable())
		m_thread.join();

	Sleep(100);
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;
}


// UDP 네트워크 쓰레드.
unsigned WINAPI UDPClientThreadFunction(void* arg)
{
	cUDPClient *udp = (cUDPClient*)arg;

// 	int snd_len;
// 	char *sndBuff = new char[udp->m_maxBuffLen];
// 	memset(sndBuff, '\0', udp->m_maxBuffLen);

	while (udp->m_threadLoop)
	{
		if (!udp->m_isConnect || (INVALID_SOCKET == udp->m_socket))
		{
			std::this_thread::sleep_for(std::chrono::microseconds(udp->m_sleepMillis));
			continue;
		}

		udp->m_sndQueue.SendAll(udp->m_sockaddr);

		//std::this_thread::sleep_for(std::chrono::microseconds(udp->m_sleepMillis));
	}

//	delete[] sndBuff;
	return 0;
}
