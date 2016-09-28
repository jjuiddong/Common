#pragma once


namespace network
{

	bool LaunchTCPClient(const std::string &ip, const int port, OUT SOCKET &out, const bool isLog = true);
	bool LaunchUDPClient(const std::string &ip, const int port, OUT SOCKADDR_IN &sockAddr, OUT SOCKET &out, const bool isLog = true);

	bool LaunchTCPServer(const int port, OUT SOCKET &out, const bool isLog = true);
	bool LaunchUDPServer(const int port, OUT SOCKET &out, const bool isLog = true);
}
