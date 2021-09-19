//
// 2021-09-19, jjuiddong
// websocket session
//	- websocket client/server with Poco Library
//		- https://pocoproject.org/
//
#pragma once


// poco class forward declare
namespace Poco {
	namespace Net {
		class WebSocket;
	}
}


namespace network2
{

	class cWebSession : public cSession
	{
	public:
		cWebSession();
		cWebSession(const netid id, const StrId &name, Poco::Net::WebSocket *ws
			, const int logId = -1);
		virtual ~cWebSession();

		virtual void Close() override;


	public:
		Poco::Net::WebSocket *m_ws;
	};

}
