//
// 2016-04-13, jjuiddong
// 일반적인 모션 믹서 기능
//
// 최종 계산된 정보
// @yaw_fin, @pitch_fin, @roll_fin, @heave_fin
//
#pragma once

#include "mixer.h"

namespace motion
{

	class cNormalMixer : public cMixer
	{
	public:
		cNormalMixer();
		virtual ~cNormalMixer();

		bool Init(const string &mixerInitScript, const string &mixerScript);
		bool Init2(const string &mixerInitScriptFileName, const string &mixerScriptFileName);
		virtual bool Start() override;
		virtual bool Stop() override;
		virtual bool Update(const float deltaSeconds) override;


	public:
		mathscript::cMathParser m_initParser;
		mathscript::cMathParser m_mixerParser;
		mathscript::cMathInterpreter m_mixerInterpreter;
	};

}
