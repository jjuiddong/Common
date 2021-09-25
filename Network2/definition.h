//
// 2019-08-31, jjuiddong
// definition
//
#pragma once


namespace network2
{

	enum {
		// netid definition
		INVALID_NETID = -1 // invalid netid
		, SERVER_NETID = 0 // client case, connected server netid
		, CLIENT_NETID = 0 // udp server case, connected client netid
		, ALL_NETID = ~1 // all connected netid, client/server all case

		// constant definition
		, RECV_BUFFER_LENGTH = 1024 * 2
		, DEFAULT_PACKETSIZE = 1024 * 1
		, DEFAULT_PACKETCOUNT = 20
		, DEFAULT_SLEEPMILLIS = 30 // milliseconds
		, DEFAULT_SOCKETBUFFER_SIZE = 1024 * 20
	};
	
	struct eProtocolType {
		enum Enum {
			TCPIP, UDPIP
		};
	};

	// packet data format
	enum class ePacketFormat {
		BINARY, // binary packet
		ASCII, // ascii packet
		JSON, // json string or binary packet
		FREE, // no format, no packet header
	};

}

