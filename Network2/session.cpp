#include "stdafx.h"
#include "session.h"

using namespace network2;


cSession::cSession()
	: m_id(0)
	, m_name("")
	, m_socket(INVALID_SOCKET)
	, m_state(eState::Disconnect)
	, m_logId(-1)
{
}

cSession::cSession(const netid id, const StrId &name
	, const int logId //= -1
)
	: m_id(id)
	, m_name(name)
	, m_socket(INVALID_SOCKET)
	, m_state(eState::Disconnect)
	, m_logId(logId)
{
	if (logId >= 0)
		network2::LogSession(logId, *this);
}

cSession::~cSession()
{
	Close();
}


void cSession::Close()
{
	if (INVALID_SOCKET != m_socket)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}


netid cSession::GetId() const
{
	return m_id;
}

bool cSession::IsConnect() const
{
	return eState::Connect == m_state;
}


bool cSession::IsReadyConnect() const
{ 
	return (eState::ReadyConnect == m_state) || (eState::TryConnect == m_state);
}


bool cSession::IsFailConnection() const
{ 
	return ((eState::Disconnect == m_state) || (eState::ConnectError == m_state));
}
