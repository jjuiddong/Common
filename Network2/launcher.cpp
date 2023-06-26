
#include "stdafx.h"
#include "launcher.h"

using namespace network2;
using namespace common;


// Start TCP Client
bool network2::LaunchTCPClient(const string ip, const int port
	, OUT SOCKET &out
	, const bool isLog // = true
	, const int clientSidePort //= -1
)
{

	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		if (isLog)
			dbg::Logc(2, "error winsock version\n");
		return false;
	}

	LPHOSTENT lpHostEntry;
	lpHostEntry = gethostbyname(ip.c_str());
	if(lpHostEntry == NULL)
	{
		if (isLog)
			dbg::Logc(2, "gethostbyname() error\n");
		return false;
	}

	const SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

	// Receive, Send Buffer
	if (1)
	{
		// https://notes.shichao.io/unp/ch7/
		// https://m.blog.naver.com/PostView.nhn?blogId=bringmelove1&logNo=119147326&proxyReferer=https%3A%2F%2Fwww.google.com%2F

		int optval;
		int optlen = sizeof(optval);
		if (getsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (char*)&optval, &optlen) < 0)
		{
			if (isLog)
				dbg::Logc(2, "setsockopt(SO_RCVBUF) failed\n");
			closesocket(clientSocket);
			return false;
		}

		optval = optval * 2;
		if (setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (char*)&optval, sizeof(optval)) < 0)
		{
			if (isLog)
				dbg::Logc(2, "setsockopt(SO_SNDBUF) failed\n");
			closesocket(clientSocket);
			return false;
		}

		if (getsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, (char*)&optval, &optlen) < 0)
		{
			if (isLog)
				dbg::Logc(2, "setsockopt(SO_RCVBUF) failed\n");
			closesocket(clientSocket);
			return false;
		}

		optval = optval * 2;
		if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval)) < 0)
		{
			if (isLog)
				dbg::Logc(2, "setsockopt(SO_RCVBUF) failed\n");
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
		if (SOCKET_ERROR == nRet)
		{
			if (isLog)
				dbg::Logc(2, "client bind() error port=%d\n", clientSidePort);
			closesocket(clientSocket);
			return false;
		}
	}

	SOCKADDR_IN saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list); // server address
	saServer.sin_port = htons(port);

	nRet = connect(clientSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr) );
	if (SOCKET_ERROR == nRet)
	{
		const DWORD errorVal = GetLastError();

		if (isLog)
			dbg::Logc(2, "connect() error ip=%s, port=%d, error=0x%x\n"
				, ip.c_str(), port, errorVal);
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
			dbg::Logc(2, "error winsock version\n");
		return false;
	}

	const SOCKET svrSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(INVALID_SOCKET == svrSocket)
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

	// Receive, Send Buffer
	if (1)
	{
		// https://notes.shichao.io/unp/ch7/
		// https://m.blog.naver.com/PostView.nhn?blogId=bringmelove1&logNo=119147326&proxyReferer=https%3A%2F%2Fwww.google.com%2F

		int optval;
		int optlen = sizeof(optval);
		if (getsockopt(svrSocket, SOL_SOCKET, SO_SNDBUF, (char*)&optval, &optlen) < 0)
		{
			if (isLog)
				dbg::Logc(2, "setsockopt(SO_RCVBUF) failed\n");
			closesocket(svrSocket);
			return false;
		}

		optval = optval * 2;
		if (setsockopt(svrSocket, SOL_SOCKET, SO_SNDBUF, (char*)&optval, sizeof(optval)) < 0)
		{
			if (isLog)
				dbg::Logc(2, "setsockopt(SO_SNDBUF) failed\n");
			closesocket(svrSocket);
			return false;
		}

		if (getsockopt(svrSocket, SOL_SOCKET, SO_RCVBUF, (char*)&optval, &optlen) < 0)
		{
			if (isLog)
				dbg::Logc(2, "setsockopt(SO_RCVBUF) failed\n");
			closesocket(svrSocket);
			return false;
		}

		optval = optval * 2;
		if (setsockopt(svrSocket, SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval)) < 0)
		{
			if (isLog)
				dbg::Logc(2, "setsockopt(SO_RCVBUF) failed\n");
			closesocket(svrSocket);
			return false;
		}
	}

	SOCKADDR_IN saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;
	saServer.sin_port = htons(port);

	nRet = bind(svrSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr) );
	if (SOCKET_ERROR == nRet)
	{
		if (isLog)
			dbg::Logc(2, "bind() error port: %d\n", port);
		closesocket(svrSocket);
		return false;
	}

	char buffer[256];
	nRet = gethostname(buffer, sizeof(buffer));
	if (SOCKET_ERROR == nRet)
	{
		if (isLog)
			dbg::Logc(2, "gethostname() error\n");
		closesocket(svrSocket);
		return false;
	}

	nRet = listen(svrSocket, SOMAXCONN);
	if (SOCKET_ERROR == nRet)
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
			dbg::Logc(2, "error winsock version\n");
		return false;
	}

	const SOCKET svrSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == svrSocket)
	{
		if (isLog)
			dbg::Logc(2, "socket() error\n");
		return false;
	}

	SOCKADDR_IN saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;
	saServer.sin_port = htons(port);

	nRet = bind(svrSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr));
	if (SOCKET_ERROR == nRet)
	{
		if (isLog)
			dbg::Logc(2, "bind() error port: %d\n", port);
		closesocket(svrSocket);
		return false;
	}

	char buffer[128];
	nRet = gethostname(buffer, sizeof(buffer));
	if (SOCKET_ERROR == nRet)
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
bool network2::LaunchUDPClient(const string ip, const int port
	, OUT SOCKADDR_IN &sockAddr, OUT SOCKET &out, const bool isLog)
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		if (isLog)
			dbg::Logc(2, "error winsock version\n");
		return false;
	}

	LPHOSTENT lpHostEntry;
	lpHostEntry = gethostbyname(ip.c_str());
	if (NULL == lpHostEntry)
	{
		if (isLog)
			dbg::Logc(2, "gethostbyname() error\n");
		return false;
	}

	const SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == clientSocket)
	{
		if (isLog)
			dbg::Logc(2, "socket() error\n");
		return false;
	}

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
	sockAddr.sin_port = htons(port);

	nRet = connect(clientSocket, (LPSOCKADDR)&sockAddr, sizeof(struct sockaddr));
	if (SOCKET_ERROR == nRet)
	{
		if (isLog)
			dbg::Logc(2, "connect() error ip=%s, port=%d\n", ip.c_str(), port);
		closesocket(clientSocket);
		return false;
	}

	out = clientSocket;

	return true;
}
