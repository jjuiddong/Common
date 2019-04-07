//
// 2019-01-08, jjuiddong
// Network Session class
//
#pragma once


namespace network2
{

	class cSession
	{
	public:
		cSession();
		cSession(const netid id, const StrId &name, const bool isPacketLog = false);
		virtual ~cSession();

		virtual void Close();
		int GetId() const;
		bool IsConnect() const;
		bool IsReadyConnect() const;
		bool IsFailConnection() const;


	public:
		enum State {
			READYCONNECT,  // 커넥션 전, 쓰레드 구동하기 직전.
			TRYCONNECT,  // 쓰레드 내에서 소켓 커넥션 요청 상태
			CONNECT,  // 커넥션 성공
			DISCONNECT, // 커넥션 실패
			CONNECT_ERROR, // 커넥션 에러, 주로 동기화 문제에서 발생
		};

		netid m_id;
		SOCKET m_socket;
		State m_state;
		StrId m_name;
		Str16 m_ip;
		int m_port;
		bool m_isPacketLog;
	};


	// Listener Change Session
	class iSessionListener
	{
	public:
		virtual void RemoveSession(cSession &session) = 0;
		virtual void AddSession(cSession &session) = 0;
	};

}
