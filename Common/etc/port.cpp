
#include "stdafx.h"
#include "port.h"
#include "enumser.h"

using namespace common;


// 
bool common::EnumCOMPort(OUT vector<std::pair<UINT, string>> &out)
{
	CEnumerateSerial enumSerial;
	enumSerial.UsingSetupAPI2(out);
	return true;
}
