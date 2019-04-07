
#include "stdafx.h"
#include "inifile.h"
#include "stringfunc.h"

using namespace common;
using namespace uiutil;


int uiutil::GetIniFileInt(const string &appName, const string &keyName, const int defaultValue
	, const string &fileName)
{
	wchar_t buff[128];
	const int ret = GetPrivateProfileStringW(
		str2wstr(appName).c_str(),
		str2wstr(keyName).c_str(),
		formatw("%d", defaultValue).c_str(),
		buff,
		(DWORD)sizeof(buff),
		str2wstr(fileName).c_str());

	return _wtoi(buff);
}


float uiutil::GetIniFileFloat(const string &appName, const string &keyName, const float defaultValue
	, const string &fileName)
{
	wchar_t buff[128];
	const int ret = GetPrivateProfileStringW(
		str2wstr(appName).c_str(),
		str2wstr(keyName).c_str(),
		formatw("%f", defaultValue).c_str(),
		buff,
		(DWORD)sizeof(buff),
		str2wstr(fileName).c_str());

	return (float)_wtof(buff);
}
