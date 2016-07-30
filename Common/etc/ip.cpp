
#include "stdafx.h"
#include "ip.h"
//#include <winsock.h>
#include <winsock2.h>


using namespace common;
using namespace std;


// 현재 컴퓨터의 IP Address를 리턴한다.
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


// 현재 컴퓨터의 모든 네트워크 카드의 IP Address를 리턴한다.
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


uint32_t parseIPV4string(const string &ipAddress) 
{
	unsigned int ipbytes[4];
	sscanf(ipAddress.c_str(), "%uhh.%uhh.%uhh.%uhh", &ipbytes[3], &ipbytes[2], &ipbytes[1], &ipbytes[0]);
	return ipbytes[0] | ipbytes[1] << 8 | ipbytes[2] << 16 | ipbytes[3] << 24;
}


void common::GetIPtoInt(const string &ip, BYTE &n1, BYTE &n2, BYTE &n3, BYTE &n4)
{
	sscanf_s(ip.c_str(), "%hhd.%hhd.%hhd.%hhd", &n1, &n2, &n3, &n4);
}
