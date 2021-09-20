
#include "stdafx.h"
#include "websession.h"
#include "Poco/Net/WebSocket.h"

using namespace network2;


cWebSession::cWebSession()
	: m_ws(nullptr)
{
}

cWebSession::cWebSession(const netid id, const StrId &name
	, Poco::Net::WebSocket *ws
	, const int logId //= -1
)
	: cSession(id, name, logId)
	, m_ws(ws)
{
	if (ws)
		m_socket = ws->impl()->sockfd();
}

cWebSession::~cWebSession()
{
	Close();
}


// close session
void cWebSession::Close()
{
	if (m_ws)
		m_ws->close();
	SAFE_DELETE(m_ws);
	m_socket = INVALID_SOCKET;
	__super::Close();
}
