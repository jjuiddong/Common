//
// 2019-08-31, jjuiddong
// definition
//
#pragma once


namespace network2
{

	enum {
		// netid definition
		INVALID_NETID = -1 // 잘못된 NetId를 뜻한다. 오류값으로 쓰임
		, SERVER_NETID = 0 // 연결된 Server의 NetId를 뜻함 (클라이언트 일 때)
		, CLIENT_NETID = 0 // 연결된 Client의 NetId를 뜻함 (UDP 서버 일 때)
		, ALL_NETID = ~1 // 연결된 Connection 모두를 뜻함 (서버/클라이언트)

		// constant definition
		, RECV_BUFFER_LENGTH = 1024
		, DEFAULT_PACKETSIZE = 512
		, DEFAULT_MAX_PACKETCOUNT = 10
		, DEFAULT_SLEEPMILLIS = 30
		, DEFAULT_SOCKETBUFFER_SIZE = 1024 * 10
	};
	
	struct eProtocolType {
		enum Enum {
			TCPIP, UDPIP
		};
	};

	// packet data format
	enum class ePacketFormat {
		BINARY, ASCII, JSON,
	};

}

