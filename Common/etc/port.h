//
// 2019-06-22, jjuiddong
// COM Port Enumeration
//
// http://www.codeproject.com/Articles/293273/Ready-to-use-serial-port-enumeration-list-box
//
#pragma once


namespace common
{

	bool EnumCOMPort(OUT vector<std::pair<UINT, string>> &out);

	bool EnumCOMPortID(OUT vector<std::tuple<UINT, string, string>> &out);

}
