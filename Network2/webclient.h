//
// 2020-11-11, jjuiddong
// WebSocket Client class with Poco Library
//	- poco library websocket client
//		- https://pocoproject.org/
//
#pragma once

// Poco library class forward declaration
namespace Poco {
	namespace Net {
		class HTTPClientSession;
		class HTTPRequest;
		class HTTPResponse;
		class WebSocket;
	}
}

namespace network2
{

	class cWebClient : public cNetworkNode
	{
	public:
		explicit cWebClient( 
			const StrId &name = "WebClient"
			, const int logId = -1
		);
		virtual ~cWebClient();

		bool Init(const string &url
			, const int packetSize = DEFAULT_PACKETSIZE
			, const int maxPacketCount = DEFAULT_MAX_PACKETCOUNT
			, const int sleepMillis = DEFAULT_SLEEPMILLIS
			, const bool isThreadMode = true
		);
		bool ReConnect();
		bool Process();
		virtual void Close() override;

		// Override
		virtual SOCKET GetSocket(const netid netId) override;
		virtual netid GetNetIdFromSocket(const SOCKET sock) override;
		virtual void GetAllSocket(OUT map<netid, SOCKET> &out) override;
		virtual int Send(const netid rcvId, const cPacket &packet) override;
		virtual int SendImmediate(const netid rcvId, const cPacket &packet) override;
		virtual int SendAll(const cPacket &packet) override { return 0; }
		virtual int SendPacket(const SOCKET sock, const cPacket &packet) override;


	protected:
		bool ConnectServer();
		static int ThreadFunction(cWebClient *client);


	public:
		// poco library object
		Poco::Net::HTTPClientSession *m_session;
		Poco::Net::HTTPRequest *m_request;
		Poco::Net::HTTPResponse *m_response;
		Poco::Net::WebSocket *m_websocket;
		//

		string m_url;
		bool m_isThreadMode;
		int m_maxBuffLen;
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;

		std::thread m_thread;
		CriticalSection m_cs;
		int m_sleepMillis;
		char *m_recvBuffer;
	};

}
