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



#include "../Common/common.h"
using namespace common;

#include <winsock2.h>

typedef int netid;

#include "definition.h"
#include "session.h"
#include "websession.h"
#include "packetheader.h"
#include "packetheaderascii.h"
#include "packetheaderjson.h"
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
#include "udpserver2.h"
#include "udpclient.h"
#include "udpclient2.h"
#include "webserver.h"
#include "webclient.h"
#include "marshalling.h"
#include "marshalling_bin.h"
#include "marshalling_ascii.h"
#include "marshalling_json.h"
#include "marshalling_script.h"
#include "protocol_udpsvrmap.h"
#include "netcontroller.h"
#include "udpservermap.h"
#include "udpservermap2.h"
#include "udpclientmap.h"
#include "protocol_basic_dispatcher.h"
#include "protocol_all_handler.h"
#include "definition_marshalling.h"
#include "sql/mysqlquery.h"

#include "prtcompiler/ProtocolDefine.h"
#include "prtcompiler/ProtocolParser.h"

#include "interpreter/remotedebugger.h"
#include "interpreter/remoteinterpreter.h"

#include "utility/all_protocol_displayer.h"
#include "utility/log.h"
#include "utility/packetlog.h"
#include "utility/sessionlog.h"
#include "utility/packetpump.h"
#include "utility/utility.h"

#include "FtpClient/FTPClient.h"

#pragma comment( lib, "wsock32.lib" )
#pragma comment(lib, "Ws2_32.lib")
