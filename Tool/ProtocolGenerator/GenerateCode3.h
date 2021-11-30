//------------------------------------------------------------------------
// 2021-08-24, jjuiddong
// 
// generate protocol source code with *.prt script
//	- generate JavaScript code
//		- protocol sender
//		- protocol handler/dispatcher
//		- protocol packet data structure
//
//------------------------------------------------------------------------
#pragma once


namespace compiler3
{

	bool WriteProtocolCode(const string &protocolFileName
		, network2::sProtocol *protocol
		, network2::sType *type
		, const bool isAsyncModule = false
		, const bool isAddFile = false
		, const string outputFileName = ""
	);

}
