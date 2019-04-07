//
// 2019-01-08, jjuiddong
// network common header
//
#pragma once

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS 
	#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#endif

#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_
#endif

#undef FD_SETSIZE
#ifndef FD_SETSIZE
	// low fd_setsize, low Performance
	#define FD_SETSIZE      1024
#endif /* FD_SETSIZE */



#include "../NetCommon/netcommon.h"
using namespace common;

#include <winsock2.h>

typedef int netid;

namespace network2 {
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
}


#include "session.h"
#include "packetheader.h"
#include "packetheaderascii.h"
#include "packet.h"
#include "factory.h"
#include "protocol.h"
#include "protocol_handler.h"
#include "protocol_dispatcher.h"
#include "protocol_macro.h"
#include "networknode.h"
#include "socketbuffer.h"
#include "packetqueue.h"
#include "launcher.h"
#include "tcpserver.h"
#include "tcpclient.h"
#include "udpserver.h"
#include "udpclient.h"
#include "netcontroller.h"
#include "marshalling_bin.h"
#include "marshalling_ascii.h"
#include "protocol_basic_dispatcher.h"
#include "protocol_all_handler.h"
#include "sql/mysqlquery.h"

#include "prtcompiler/ProtocolDefine.h"
#include "prtcompiler/ProtocolParser.h"
#include "prtcompiler/protocolutil.h"

#include "utility/all_protocol_displayer.h"
#include "utility/log.h"
#include "utility/packetlog.h"
#include "utility/sessionlog.h"


#pragma comment( lib, "wsock32.lib" )
#pragma comment(lib, "Ws2_32.lib")
