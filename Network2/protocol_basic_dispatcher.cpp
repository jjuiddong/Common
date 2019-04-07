
#include "stdafx.h"
#include "protocol_basic_dispatcher.h"

using namespace network2;
using namespace basic_protocol;


// 서버와 관련된 기본적인 작업을 처리한다.
// Connect, Disconnect, Accept를 패킷화 해서 처리한다.
void basic_protocol::ServerDispatcher::Dispatch(cPacket &packet, cTcpServer *svr)
{
	RET(!svr);

	switch (packet.GetPacketId())
	{
	case PACKETID_DISCONNECT:
	{
		netid disconnectId = INVALID_NETID;
		packet >> disconnectId;
		if (svr->GetId() == disconnectId)
		{
			svr->Close();
		}
		else
		{
			svr->RemoveSession(disconnectId);
		}
	}
	break;

	case PACKETID_CLIENT_DISCONNECT:
	{
		netid disconnectId = INVALID_NETID;
		packet >> disconnectId;
		svr->RemoveSession(disconnectId);
	}
	break;

	case PACKETID_ACCEPT:
	{
		SOCKET remoteClientSock;
		packet >> remoteClientSock;
		std::string ip;
		packet >> ip;
		int port;
		packet >> port;
		svr->AddSession(remoteClientSock, ip, port);
	}
	break;
	}
}


// 클라이언트와 관련된 기본적인 작업을 처리한다.
// Disconnect 를 패킷화 해서 처리한다.
void basic_protocol::ClientDispatcher::Dispatch(cPacket &packet, cTcpClient *client)
{
	RET(!client);

	switch (packet.GetPacketId())
	{
	case PACKETID_DISCONNECT:
	{
		client->Close();
	}
	break;
	}
}

