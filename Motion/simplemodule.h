//
// 2016-04-17, jjuiddong
// 모듈레이터 클래스
//
#pragma once

#include "module.h"


namespace motion
{

	class cSimpleModule : public cModule
	{
	public:
		cSimpleModule();
		virtual ~cSimpleModule();

		bool Init(const string &inputValue, const string &outputValue, const string &mathScript, const string &modulatorScript);
		bool Init2(const string &inputValue, const string &outputValue, const string &mathScriptFileName, const string &modulatorScriptFileName);
		virtual bool Start() override;
		virtual bool Stop() override;
		virtual bool Update(const float deltaSeconds) override;


	public:
		string m_inputValue;
		string m_outputValue;
		mathscript::cMathParser m_mathParser;
		mathscript::cMathInterpreter m_matInterpreter;
		cSimpleModulator m_modulator;
	};

}
