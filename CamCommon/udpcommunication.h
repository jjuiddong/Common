#pragma once


class cUDPCommunication
{
public:
	cUDPCommunication();
	virtual ~cUDPCommunication();

	void Init(const int port);
	void SendData(const sSharedData &data);


	SOCKET m_socket;
	sockaddr_in m_si_other;
	HANDLE m_handle;
	DWORD m_threadId;
	sSharedData m_sharedData;
	CRITICAL_SECTION m_CriticalSection;
	bool m_threadLoop;
};
