
#include "stdafx.h"
#include "amplitudemodule.h"

using namespace motion;


cAmplitudeModule::cAmplitudeModule()
	: cModule()
{
}

cAmplitudeModule::~cAmplitudeModule()
{
}


bool cAmplitudeModule::Init(const string &mathScript, const string &modulatorScript)
{
	if (!m_mathParser.ParseStr(mathScript))
		return false;

	if (modulatorScript.empty())
	{
		// 기본값 설정
		m_modulator.Init();
	}
	else
	{
		m_modulator.ParseStr(modulatorScript);
	}

	m_modulator.m_axis.isPIClamp = false;

	return true;
}


bool cAmplitudeModule::Start()
{
	m_state = MODULE_STATE::START;
	return true;
}


bool cAmplitudeModule::Stop()
{
	m_state = MODULE_STATE::STOP;
	return true;
}


bool cAmplitudeModule::Update(const float deltaSeconds)
{
	RETV(m_state == MODULE_STATE::STOP, false);

	m_incTime += deltaSeconds;
	if (m_incTime < 0.033f)
		return true;

	// 현재 Axis 값을 가져온다.
	const Vector3 axis = Vector3(script::g_symbols["@roll_fin"].fVal,
		script::g_symbols["@pitch_fin"].fVal,
		script::g_symbols["@yaw_fin"].fVal);
	
	const Vector3 v = axis - m_prevAxis;
	const float amplitude = v.Length();

	m_modulator.Update(m_incTime, amplitude);

	float out;
	m_modulator.GetFinal(out);

	const Vector3 diff = v.Normal() * out;
	const Vector3 val = m_prevAxis + diff;
	m_prevAxis = val; // update current value
	
	// Update Symbols
	script::sFieldData data;
	data.type = script::FIELD_TYPE::T_FLOAT;
	data.fVal = amplitude;
	script::g_symbols["@amplitude0"] = data;
	data.fVal = out;
	script::g_symbols["@amplitude"] = data;

	data.type = script::FIELD_TYPE::T_INT;
	data.iVal = (int)val.x;
	script::g_symbols["@roll_fin"] = data;
	data.iVal = (int)val.y;
	script::g_symbols["@pitch_fin"] = data;
	data.iVal = (int)val.z;
	script::g_symbols["@yaw_fin"] = data;

	// Excute Math Script
	m_matInterpreter.Excute(m_mathParser.m_stmt);

	m_incTime = 0;

	return true;
}
