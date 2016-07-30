
#include "stdafx.h"
#include "simplemodule.h"

using namespace motion;


cSimpleModule::cSimpleModule() 
	: cModule()
{
}

cSimpleModule::~cSimpleModule()
{
}


bool cSimpleModule::Init(const string &inputValue, const string &outputValue, 
	const string &mathScript, const string &modulatorScript)
{
	m_inputValue = inputValue;
	m_outputValue = outputValue;

	if (!m_mathParser.ParseStr(mathScript))
		return false;

	m_modulator.ParseStr(modulatorScript);

	return true;
}


bool cSimpleModule::Init2(const string &inputValue, const string &outputValue, 
	const string &mathScriptFileName, const string &modulatorScriptFileName)
{
	m_inputValue = inputValue;
	m_outputValue = outputValue;

	if (!m_mathParser.Read(mathScriptFileName))
		return false;

	if (!m_modulator.Read(modulatorScriptFileName))
		return false;

	return true;
}


bool cSimpleModule::Start()
{
	m_state = MODULE_STATE::START;
	return true;
}


bool cSimpleModule::Stop()
{
	m_state = MODULE_STATE::STOP;
	return true;
}


bool cSimpleModule::Update(const float deltaSeconds)
{
	RETV(m_inputValue.empty(), false);
	RETV(m_outputValue.empty(), false);
	RETV(m_state == MODULE_STATE::STOP, false);

	m_incTime += deltaSeconds;
	if (m_incTime < 0.033f)
		return true;

	// Excute Math Script
	m_matInterpreter.Excute(m_mathParser.m_stmt);

	const float in = script::g_symbols[m_inputValue].fVal;
	m_modulator.Update(m_incTime, in);

	float out;
	m_modulator.GetFinal(out);

	script::sFieldData data;
	data.type = script::FIELD_TYPE::T_FLOAT;
	data.fVal = out;
	script::g_symbols[m_outputValue] = data;

	m_incTime = 0;
	return true;
}
