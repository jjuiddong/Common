//------------------------------------------------------------------------
// 2021-08-22, jjuiddong
// 
// generate protocol source code with *.prt script
//	- generate TypeScript code
//		- protocol sender
//		- protocol handler/dispatcher
//		- protocol packet data structure
//
//------------------------------------------------------------------------
#pragma once


namespace compiler2
{

	bool WriteProtocolCode(const string &protocolFileName
		, network2::sProtocol *protocol
		, network2::sType *type
		, const bool isAsyncModule = false
	);

}
