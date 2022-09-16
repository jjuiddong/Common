//------------------------------------------------------------------------
// 2021-11-30, jjuiddong
// 
// generate protocol source code with *.prt script
//	- generate JavaScript code
//		- merge js code
//		- packet
//
//------------------------------------------------------------------------
#pragma once


namespace compiler4
{

	string WriteProtocolCode1(const string &outputFileName);
	bool WriteProtocolCode2(const string &dirPath
		, const string &outputFileName
		, const vector<string> &dispatchers
		, const vector<string> &handlers);

}
