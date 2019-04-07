//
// 2016-04-26, jjuiddong
//
// *.ini 파일 정보를 읽어오는 기능을 모았다.
//
#pragma once

#include "stringfunc.h"


namespace uiutil
{

	using namespace common;
	using std::string;

	int GetIniFileInt(const string &appName, const string &keyName, const int defaultValue
		, const string &fileName);

	float GetIniFileFloat(const string &appName, const string &keyName, const float defaultValue
		, const string &fileName);

	template <size_t MAX>
	string GetIniFileString(const string &appName, const string &keyName, const string &defaultValue
		, const string &fileName);

}


template <size_t MAX = 128>
inline string uiutil::GetIniFileString(const string &appName, const string &keyName, const string &defaultValue
	, const string &fileName)
{
	char buff[MAX];
	const int ret = GetPrivateProfileStringA(appName.c_str(), keyName.c_str(), defaultValue.c_str(),
		buff, (DWORD)sizeof(buff), fileName.c_str());
	return buff;
}
