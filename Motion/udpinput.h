//
// 2016-04-13, jjuiddong
// UDP 통신으로 모션 정보를 받아 처리한다.
//
#pragma once

#include "input.h"


namespace motion
{

	class cUDPInput : public cInput
	{
	public:
		cUDPInput();
		virtual ~cUDPInput();

		bool Init(const int startIndex, const int udpBindPort, const string &protocolCmd, const string &cmd, const string &modulatorScript);
		bool Init2(const int startIndex, const int udpBindPort, const string &protocolScriptFileName,
			const string &cmdScriptFileName, const string &modulatorScriptFileName);
		virtual bool Start() override;
		virtual bool Stop() override;
		virtual bool Update(const float deltaSeconds) override;


	protected:
		void ParseUDPData(const BYTE *buffer, const int bufferLen);


	public:
		int m_udpBindPort;
		int m_startIndex;
		network::cUDPServer m_udpSvr;
		cProtocolParser m_protocolParser;
		cMathParser m_mathParser;
		mathscript::cMathInterpreter m_matInterpreter;
		cMotionWaveModulator m_modulator;
	};

}
