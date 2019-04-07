
#include "stdafx.h"
#include "launcher.h"

using namespace network2;
using namespace common;


// Start TCP Client
bool network2::LaunchTCPClient(const std::string &ip, const int port
	, OUT SOCKET &out
	, const bool isLog // = true
	, const int clientSidePort //= -1
)
{
	const string tmpIp = ip; // thread safety

	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		if (isLog)
			dbg::Logc(2, "윈속 버전이 틀렸습니다\n");
		return false;
	}

	LPHOSTENT lpHostEntry;
	lpHostEntry = gethostbyname(tmpIp.c_str());
	if(lpHostEntry == NULL)
	{
		if (isLog)
			dbg::Logc(2, "gethostbyname() error\n");
		return false;
	}

	// socket(주소 계열, 소켓 형태, 프로토콜)
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		if (isLog)
			dbg::Logc(2, "socket() error\n");
		return false;
	}

	linger lin;
	lin.l_onoff = 0;
	lin.l_linger = 0;
	if (setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, (const char*)&lin, sizeof(int)) < 0)
	{
		if (isLog)
			dbg::Logc(2, "setsockopt(SO_LINGER) failed\n");
		closesocket(clientSocket);
		return false;
	}

	// Nagle Algorithm On/Off
	if (0)
	{
		int opt_val = 1; // Nagle Algorithm Off
		if (setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt_val, sizeof(int)) < 0)
		{
			if (isLog)
				dbg::Logc(2, "setsockopt(TCP_NODELAY) failed\n");
			closesocket(clientSocket);
			return false;
		}
	}

	// Client Side Port Setting
	// https://stackoverflow.com/questions/18050065/specifying-port-number-on-client-side
	if (clientSidePort > 0)
	{
		SOCKADDR_IN saClient;
		saClient.sin_family = AF_INET;
		saClient.sin_addr.s_addr = INADDR_ANY;
		saClient.sin_port = htons(clientSidePort);
		nRet = bind(clientSocket, (LPSOCKADDR)&saClient, sizeof(struct sockaddr));
		if (nRet == SOCKET_ERROR)
		{
			if (isLog)
				dbg::Logc(2, "client bind() error port=%d\n", clientSidePort);
			closesocket(clientSocket);
			return false;
		}
	}

	SOCKADDR_IN saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list); // 서버 주소
	saServer.sin_port = htons(port);

	// connect(소켓, 서버 주소, 서버 주소의 길이
	nRet = connect(clientSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr) );
	if (nRet == SOCKET_ERROR)
	{
		const DWORD errorVal = GetLastError();

		if (isLog)
			dbg::Logc(2, "connect() error ip=%s, port=%d, error=0x%x\n", ip.c_str(), port, errorVal);
		closesocket(clientSocket);
		return false;
	}

	out = clientSocket;

	return true;
}


// Start TCP Server
bool network2::LaunchTCPServer(const int port, OUT SOCKET &out, const bool isLog)
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		if (isLog)
			dbg::Logc(2, "윈속 버전이 틀렸습니다\n");
		return false;
	}

	// socket(주소계열, 소켓 형식, 프로토콜)
	SOCKET svrSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(svrSocket == INVALID_SOCKET)
	{
		if (isLog)
			dbg::Logc(2, "socket() error\n");
		return false;
	}

	// Nagle Algorithm On/Off
	if (0)
	{
		int opt_val = 1; // Nagle Algorithm Off
		if (setsockopt(svrSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt_val, sizeof(int)) < 0)
		{
			if (isLog)
				dbg::Logc(2, "setsockopt(TCP_NODELAY) failed\n");
			closesocket(svrSocket);
			return false;
		}
	}

	SOCKADDR_IN saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;
	saServer.sin_port = htons(port);

	// bind(소켓, 서버 주소, 주소 구조체의 길이
	nRet = bind(svrSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr) );
	if (nRet == SOCKET_ERROR)
	{
		if (isLog)
			dbg::Logc(2, "bind() error port: %d\n", port);
		closesocket(svrSocket);
		return false;
	}

	char szBuf[256];
	nRet = gethostname( szBuf, sizeof(szBuf) );
	if (nRet == SOCKET_ERROR)
	{
		if (isLog)
			dbg::Logc(2, "gethostname() error\n");
		closesocket(svrSocket);
		return false;
	}

	// listen(오는 소켓, 요청 수용 가능한 용량)
	nRet = listen(svrSocket, SOMAXCONN);

	if (nRet == SOCKET_ERROR)
	{
		if (isLog)
			dbg::Logc(2, "listen() error\n");
		closesocket(svrSocket);
		return false;
	}

	out = svrSocket;

	return true;
}


// Start UDP Server
bool network2::LaunchUDPServer(const int port, OUT SOCKET &out, const bool isLog)
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		if (isLog)
			dbg::Logc(2, "윈속 버전이 틀렸습니다\n");
		return false;
	}

	// socket(주소계열, 소켓 형식, 프로토콜)
	SOCKET svrSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (svrSocket == INVALID_SOCKET)
	{
		if (isLog)
			dbg::Logc(2, "socket() error\n");
		return false;
	}

	SOCKADDR_IN saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;
	saServer.sin_port = htons(port);

	// bind(소켓, 서버 주소, 주소 구조체의 길이)
	nRet = bind(svrSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr));
	if (nRet == SOCKET_ERROR)
	{
		if (isLog)
			dbg::Logc(2, "bind() error port: %d\n", port);
		closesocket(svrSocket);
		return false;
	}

	char szBuf[256];
	nRet = gethostname(szBuf, sizeof(szBuf));
	if (nRet == SOCKET_ERROR)
	{
		if (isLog)
			dbg::Logc(2, "gethostname() error\n");
		closesocket(svrSocket);
		return false;
	}

	out = svrSocket;

	return true;
}


// Start UDP Client
bool network2::LaunchUDPClient(const std::string &ip, const int port
	, OUT SOCKADDR_IN &sockAddr, OUT SOCKET &out, const bool isLog)
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		if (isLog)
			dbg::Logc(2, "윈속 버전이 틀렸습니다\n");
		return false;
	}

	LPHOSTENT lpHostEntry;
	lpHostEntry = gethostbyname(ip.c_str());
	if (lpHostEntry == NULL)
	{
		if (isLog)
			dbg::Logc(2, "gethostbyname() error\n");
		return false;
	}

	// socket(주소 계열, 소켓 형태, 프로토콜)
	SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		if (isLog)
			dbg::Logc(2, "socket() error\n");
		return false;
	}

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
	sockAddr.sin_port = htons(port);

	// connect(소켓, 서버 주소, 서버 주소의 길이)
	nRet = connect(clientSocket, (LPSOCKADDR)&sockAddr, sizeof(struct sockaddr));
	if (nRet == SOCKET_ERROR)
	{
		if (isLog)
			dbg::Logc(2, "connect() error ip=%s, port=%d\n", ip.c_str(), port);
		closesocket(clientSocket);
		return false;
	}

	out = clientSocket;

	return true;
}
