
#include "stdafx.h"
#include "protocol_basic_dispatcher.h"

using namespace network2;
using namespace basic_protocol;


// server packet basic dispatcher
// Connect, Disconnect, Accept packet handler
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


// client basic packet dispatcher
// Disconnect packet dispatch
void basic_protocol::ClientDispatcher::Dispatch(cPacket &packet, cTcpClient *client)
{
	RET(!client);

	switch (packet.GetPacketId())
	{
	case PACKETID_DISCONNECT:
	{
		client->RemoveSession();		
	}
	break;
	}
}


// server packet basic dispatcher
// Connect, Disconnect, Accept packet handler
void basic_protocol::WebServerDispatcher::Dispatch(cPacket &packet, cWebServer *svr)
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


// webclient basic packet dispatch
// Disconnect packet dispatch
void basic_protocol::WebClientDispatcher::Dispatch(cPacket &packet, cWebClient *client)
{
	RET(!client);

	switch (packet.GetPacketId())
	{
	case PACKETID_DISCONNECT:
	{
		client->RemoveSession();
	}
	break;
	}
}

