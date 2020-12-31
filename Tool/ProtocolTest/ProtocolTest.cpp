
#include "stdafx.h"
#include "../Protocol/Src/gps_Protocol.h"
#include "../Protocol/Src/gps_ProtocolHandler.h"

class cTest : public gps::c2s_ProtocolHandler
{
public:
	cTest() {
		m_client.AddProtocolHandler(this);
		m_client.RegisterProtocol(&m_protocol);
	}
	virtual ~cTest() {

	}

public:
	virtual bool GPSInfo(gps::GPSInfo_Packet &packet) 
	{ 
		std::cout << "Recv GPSInfo\n";
		return true;
	}
	
	gps::c2s_Protocol m_protocol;
	network2::cWebClient m_client;
};

bool g_isLoop = true;
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	g_isLoop = false;
	return TRUE;
}


int main()
{
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		std::cout << "SetConsoleCtrlHandler failed, code : " << GetLastError() << std::endl;
		return -1;
	}

	cTest test;
	network2::cNetController netController;
	if (!netController.StartWebClient(&test.m_client, "jjuiddong.iptime.org", 4001))
	{
		std::cout << "error connect web client" << std::endl;
		return 0;
	}

	common::cTimer timer;
	timer.Create();

	double oldT = timer.GetSeconds();
	double msgT = 0.f;
	while (g_isLoop && !test.m_client.IsFailConnection())
	{
		const double curT = timer.GetSeconds();
		const float dt = (float)(curT - oldT);
		oldT = curT;
		netController.Process(dt);

		msgT += dt;
		if (msgT > 5.0f)
		{
			msgT = 0;
			test.m_protocol.GPSInfo(network2::SERVER_NETID, false
				, oldT, 2.0f, 3.0f, 4.0f, "test");
		}

		Sleep(1);
	}

	netController.Clear();
	network2::DisplayPacketCleanup();
}
