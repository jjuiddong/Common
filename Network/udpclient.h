//
// v.1
// udp client, send, receive ±â´É
//
// v.2
// close(), waitforsingleobject
// isConnect()
// thread restart bug fix
//
#pragma once


namespace network
{

	class cUDPClient
	{
	public:
		cUDPClient();
		virtual ~cUDPClient();

		bool Init(const string &ip, const int port, const int sleepMillis=30);
		void SendData(const BYTE *buff, const int buffLen);
		int GetReceiveData(BYTE *dst, const int maxbuffLen);
		void SetMaxBufferLength(const int length);
		void Close();
		bool IsConnect() const;


		string m_ip;
		int m_port;

		bool m_isConnect;
		SOCKET m_socket;
		sockaddr_in m_si_other;
		BYTE *m_sndBuffer;
		int m_sndBuffLen;
		int m_maxBuffLen;
		BYTE *m_rcvBuffer;
		int m_rcvBuffLen;
		bool m_isReceiveData;
		bool m_isSendData;

		HANDLE m_handle;
		DWORD m_threadId;
		CRITICAL_SECTION m_sndCriticalSection;
		CRITICAL_SECTION m_rcvCriticalSection;
		bool m_threadLoop;
		int m_sleepMillis; // Sleep(m_sleepMillis)
	};


	inline bool cUDPClient::IsConnect() const { return m_isConnect; }

}
