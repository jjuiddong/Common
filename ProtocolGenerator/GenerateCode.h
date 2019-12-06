//------------------------------------------------------------------------
// Name:    GenerateProtocolCode.h
// Author:  jjuiddong
// Date:    12/26/2012
// 
// 프로토콜 구문트리로 Protocol 소스 파일을 생성한다.
//------------------------------------------------------------------------
#pragma once

namespace compiler
{

	bool WriteProtocolCode(const string &protocolFileName
		, network2::sRmi *rmi
		, const string &pchFileName, const string &marshallingName);

}
