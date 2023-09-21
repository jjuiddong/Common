
#include "stdafx.h"
#include "ip.h"
#include <winsock2.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

using namespace common;
using namespace std;


// return host internal ip address
string common::GetHostIP(const int networkCardIndex)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		return "";
	}

	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) 
	{
		return "";
	}

	struct hostent *phe = gethostbyname(ac);
	if (phe == 0) 
	{
		return "";
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i)
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));

		if (networkCardIndex == i)
			return inet_ntoa(addr);
	}

	WSACleanup();

	return "";
}


// return host external ip address
// https://stackoverflow.com/questions/39566240/how-to-get-the-external-ip-address-in-c
string common::GetHostRealIp()
{
	HINTERNET net = InternetOpenA("IP retriever",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0);
	if (!net)
		return ""; // error return

	HINTERNET conn = InternetOpenUrlA(net,
		//"http://myexternalip.com/raw",
		"http://ipecho.net/plain",
		NULL,
		0,
		INTERNET_FLAG_RELOAD,
		0);
	if (!conn)
		return ""; // error return

	char buffer[4096];
	DWORD read;
	InternetReadFile(conn, buffer, ARRAYSIZE(buffer), &read);
	InternetCloseHandle(net);

	return string(buffer, read);
}


// return all host ip (two more network card)
void common::GetHostIPAll(vector<string> &ips)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		return;
	}

	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR)
	{
		return;
	}

	struct hostent *phe = gethostbyname(ac);
	if (phe == 0)
	{
		return;
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i)
	{
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		ips.push_back(inet_ntoa(addr));
	}

	WSACleanup();
}


uint32_t common::parseIPV4string(const string &ipAddress) 
{
	unsigned int ipbytes[4];
	sscanf(ipAddress.c_str(), "%uhh.%uhh.%uhh.%uhh", &ipbytes[3], &ipbytes[2], &ipbytes[1], &ipbytes[0]);
	return ipbytes[0] | ipbytes[1] << 8 | ipbytes[2] << 16 | ipbytes[3] << 24;
}


void common::GetIPtoInt(const string &ip, BYTE &n1, BYTE &n2, BYTE &n3, BYTE &n4)
{
	sscanf_s(ip.c_str(), "%hhd.%hhd.%hhd.%hhd", &n1, &n2, &n3, &n4);
}
