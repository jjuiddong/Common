
#include "stdafx.h"
#include "joystickinput.h"

using namespace motion;


cJoyStickInput::cJoyStickInput()
	: cInput(MOTION_MEDIA::JOYSTICK)
	, m_device(JOYSTICK)
	, m_hWnd(NULL)
{
}

cJoyStickInput::~cJoyStickInput()
{
}


bool cJoyStickInput::Init(const DEVICE device, const HWND hWnd, const string &mathStr, const string &modulatorScript)
{
	m_device = device;
	m_hWnd = hWnd;

	if (!m_mathParser.ParseStr(mathStr))
		return false;

	m_modulator.ParseStr(modulatorScript);

	return true;
}


bool cJoyStickInput::Init2(const DEVICE device, const HWND hWnd, const string &mathScriptFileName, const string &modulatorScriptFileName)
{
	m_device = device;
	m_hWnd = hWnd;

	if (!m_mathParser.Read(mathScriptFileName))
		return false;

	if (!m_modulator.Read(modulatorScriptFileName))
		return false;

	return true;
}


bool cJoyStickInput::Start()
{
	//if (!m_joyPad.Init(m_hWnd))
	//{
	//	dbg::ErrLog("Error!! Init JoyPad \n");
	//	return false;
	//}

	m_state = MODULE_STATE::START;

	return true;
}


bool cJoyStickInput::Stop()
{
	m_state = MODULE_STATE::STOP;
	//m_joyPad.Clear();

	return true;
}


bool cJoyStickInput::Update(const float deltaSeconds)
{
	RETV(m_state == MODULE_STATE::STOP, false);

	m_incTime += deltaSeconds;
	if (m_incTime < 0.033f)
		return true;

	//m_joyPad.Update(deltaSeconds);
	
	// Joystick 데이타 저장
	//{
	//	script::sFieldData data;
	//	data.type = script::FIELD_TYPE::T_FLOAT;
	//	data.fVal = (float)m_joyPad.m_axisX;
	//	script::g_symbols["@xaxis"] = data;
	//	data.fVal = (float)m_joyPad.m_axisY;
	//	script::g_symbols["@yaxis"] = data;
	//	data.fVal = (float)m_joyPad.m_axisZ;
	//	script::g_symbols["@zaxis"] = data;

	//	data.fVal = (float)m_joyPad.m_axisRx;
	//	script::g_symbols["@xrot"] = data;
	//	data.fVal = (float)m_joyPad.m_axisRy;
	//	script::g_symbols["@yrot"] = data;
	//	data.fVal = (float)m_joyPad.m_axisRz;
	//	script::g_symbols["@zrot"] = data;

	//	data.fVal = (float)m_joyPad.m_axisH;
	//	script::g_symbols["@slider"] = data;
	//}

	// Excute Math Script
	m_matInterpreter.Excute(m_mathParser.m_stmt);

	// Calc Modulation
	const float roll = script::g_symbols["@roll"].fVal;
	const float pitch = script::g_symbols["@pitch"].fVal;
	const float yaw = script::g_symbols["@yaw"].fVal;
	const float heave = script::g_symbols["@heave"].fVal;
	m_modulator.Update(m_incTime, yaw, pitch, roll, heave);

	// Save Symboltable Modulation Output Data
	float froll, fpitch, fyaw, fheave;
	m_modulator.GetFinal(fyaw, fpitch, froll, fheave);

	{
		script::sFieldData data;
		data.type = script::FIELD_TYPE::T_FLOAT;
		data.fVal = froll;
		script::g_symbols["@roll_mod"] = data;
		data.fVal = fpitch;
		script::g_symbols["@pitch_mod"] = data;
		data.fVal = fyaw;
		script::g_symbols["@yaw_mod"] = data;
		data.fVal = fheave;
		script::g_symbols["@heave_mod"] = data;
	}

	return true;
}
