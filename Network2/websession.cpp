
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
}

cWebSession::~cWebSession()
{
	Close();
}


// close session
void cWebSession::Close()
{
	SAFE_DELETE(m_ws);
}
