#include "stdafx.h"
#include "session.h"

using namespace network2;


cSession::cSession()
	: m_id(0)
	, m_name("")
	, m_socket(INVALID_SOCKET)
	, m_state(DISCONNECT)
	, m_isPacketLog(false)
{
}

cSession::cSession(const netid id, const StrId &name
	, const bool isPacketLog //= false
)
	: m_id(id)
	, m_name(name)
	, m_socket(INVALID_SOCKET)
	, m_state(DISCONNECT)
	, m_isPacketLog(isPacketLog)
{
	if (isPacketLog)
		network2::LogSession(*this);
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


int cSession::GetId() const
{
	return m_id;
}

bool cSession::IsConnect() const
{
	return m_state == CONNECT;
}


bool cSession::IsReadyConnect() const
{ 
	return (m_state == READYCONNECT) || (m_state == TRYCONNECT); 
}


bool cSession::IsFailConnection() const
{ 
	return (m_state == DISCONNECT); 
}
