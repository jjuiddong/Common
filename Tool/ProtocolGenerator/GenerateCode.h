//------------------------------------------------------------------------
// Name:    GenerateProtocolCode.h
// Author:  jjuiddong
// Date:    12/26/2012
// 
// generate protocol source code with *.prt script
//	- generate C++ code
//		- protocol sender
//		- protocol handler/dispatcher
//		- protocol packet data structure
//
// 2020-11-10
//	- add json format string send/recv
//	- remove marshallingName
//
// 2021-08-22
//	- refactoring
//
//------------------------------------------------------------------------
#pragma once


namespace compiler
{

	bool WriteProtocolCode(const string &protocolFileName
		, network2::sProtocol *protocol
		, const string &pchFileName
	);

}
