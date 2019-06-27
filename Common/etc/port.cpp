
#include "stdafx.h"
#include "port.h"
#include "enumser.h"

using namespace common;


// 
bool common::EnumCOMPort(OUT vector<std::pair<UINT, string>> &out)
{

#ifdef _UNICODE
	vector<std::pair<UINT, wstring>> result;
	CEnumerateSerial enumSerial;
	enumSerial.UsingSetupAPI2(result);
	for (auto &v : result)
		out.push_back({ v.first, wstr2str(v.second) });
#else
	CEnumerateSerial enumSerial;
	enumSerial.UsingSetupAPI2(out);
#endif
	return true;
}
