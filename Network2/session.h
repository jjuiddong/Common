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
		cSession(const netid id, const StrId &name, const int logId = -1);
		virtual ~cSession();

		virtual void Close();
		int GetId() const;
		bool IsConnect() const;
		bool IsReadyConnect() const;
		bool IsFailConnection() const;


	public:
		enum class eState {
			ReadyConnect,  // before connect, before start thread
			TryConnect,  // try connect
			Connect,  // success connect
			Disconnect, // close connect
			ConnectError, // connect error
		};

		netid m_id;
		SOCKET m_socket;
		eState m_state;
		StrId m_name;
		Str16 m_ip;
		int m_port;
		int m_logId; // packet log id, default: -1
	};


	// Change Session State Listener
	// server side: accept, disconnect session
	// client side: connect, disconnect session
	class iSessionListener
	{
	public:
		virtual bool AddSession(network2::cSession &session) = 0;
		virtual bool RemoveSession(network2::cSession &session) = 0;
		virtual bool ErrorSession(network2::cSession &session) { return true; }
	};

}
