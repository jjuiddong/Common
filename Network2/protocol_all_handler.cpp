
#include "stdafx.h"
#include "protocol_all_handler.h"

using namespace network2;
using namespace all;


bool Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	SEND_HANDLER(ProtocolHandler, handlers, RecvPacket(packet));
	return true;
}
