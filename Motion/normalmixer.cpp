
#include "stdafx.h"
#include "normalmixer.h"

using namespace motion;

cNormalMixer::cNormalMixer()
{
}

cNormalMixer::~cNormalMixer()
{
}


bool cNormalMixer::Init(const string &mixerInitScript, const string &mixerScript)
{
	if (!m_initParser.ParseStr(mixerInitScript))
		return false;

	if (!m_mixerParser.ParseStr(mixerScript))
		return false;

	return true;
}


bool cNormalMixer::Init2(const string &mixerInitScriptFileName, const string &mixerScriptFileName)
{
	if (!m_initParser.Read(mixerInitScriptFileName))
		return false;

	if (!m_mixerParser.Read(mixerScriptFileName))
		return false;

	return true;
}


bool cNormalMixer::Start()
{
	m_state = MODULE_STATE::START;

	// :init 스크립트는 시작될 때, 한번 만 실행된다.
	m_mixerInterpreter.Excute(m_initParser.m_stmt);

	return true;
}

bool cNormalMixer::Stop()
{
	m_state = MODULE_STATE::STOP;
	return true;
}


bool cNormalMixer::Update(const float deltaSeconds)
{
	RETV(MODULE_STATE::STOP == m_state, false);

	m_incTime += deltaSeconds;
	if (m_incTime < 0.033f)
		return true;

	m_mixerInterpreter.Excute(m_mixerParser.m_stmt);

	m_incTime = 0;
	return true;
}
