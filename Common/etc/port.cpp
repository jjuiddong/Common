
#include "stdafx.h"
#include "port.h"
#include "enumser.h"

using namespace common;


// 
bool common::EnumCOMPort(OUT vector<std::pair<UINT, string>> &out)
{
#ifdef _UNICODE
	vector<std::pair<UINT, wstring>> result;
	CEnumerateSerial::UsingSetupAPI2(result);
	for (auto &v : result)
		out.push_back({ v.first, wstr2str(v.second) });
#else
	 CEnumerateSerial::UsingSetupAPI2(out);
#endif
	return true;
}


// tuple< portnum, port name, device id >
bool common::EnumCOMPortID(OUT vector<std::tuple<UINT, string, string>> &out)
{
#ifdef _UNICODE
	vector<std::tuple<UINT, wstring, wstring>> result;
	CEnumerateSerial::UsingSetupAPI2_Ids(result);
	for (auto &v : result)
		out.push_back({ std::get<0>(v)
			, wstr2str(std::get<1>(v))
			, wstr2str(std::get<2>(v)) });
#else
	CEnumerateSerial::UsingSetupAPI2_Ids(out);
#endif
	return true;
}
