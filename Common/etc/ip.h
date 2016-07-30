//
// IP Address
//
#pragma once

#include <vector>
#include <string>

namespace common
{

	void GetIPtoInt(const string &ip, BYTE &n1, BYTE &n2, BYTE &n3, BYTE &n4);

	string GetHostIP(const int networkCardIndex);

	void GetHostIPAll(std::vector<std::string> &ips);

	uint32_t parseIPV4string(const string &ipAddress);

}

