
#include "stdafx.h"
#include "udpinput.h"


using namespace motion;


cUDPInput::cUDPInput()
	: cInput(MOTION_MEDIA::UDP)
	, m_udpBindPort(0)
	, m_startIndex(1)
{
}

cUDPInput::~cUDPInput()
{
}


// 클래스 초기화
bool cUDPInput::Init(const int startIndex, const int udpBindPort, const string &protocolCmd, const string &cmd, const string &modulatorScript)
{
	m_startIndex = startIndex;
	m_udpBindPort = udpBindPort;

	if (!m_protocolParser.ParseStr(protocolCmd, "\n"))
		return false;

	if (!m_mathParser.ParseStr(cmd))
		return false;

	m_modulator.ParseStr(modulatorScript);

	return true;
}


// 클래스 초기화
bool cUDPInput::Init2(const int startIndex, const int udpBindPort, const string &protocolScriptFileName,
	const string &cmdScriptFileName, const string &modulatorScriptFileName)
{
	m_startIndex = startIndex;
	m_udpBindPort = udpBindPort;

	if (!m_protocolParser.Read(protocolScriptFileName, "\n"))
		return false;

	if (!m_mathParser.Read(cmdScriptFileName))
		return false;

	if (!m_modulator.Read(modulatorScriptFileName))
		return false;

	return false;
}


// UDP 입력 시작
bool cUDPInput::Start()
{
	RETV((m_udpBindPort == 0), false);

	m_udpSvr.m_sleepMillis = 30;
	if (!m_udpSvr.Init(0, m_udpBindPort, 256, 1))
		return false;

	m_state = MODULE_STATE::START;

	return true;
}


// UDP 입력 종료
bool cUDPInput::Stop()
{
	m_udpSvr.Close();

	m_state = MODULE_STATE::STOP;

	return true;
}


// UDP 입력 처리
bool cUDPInput::Update(const float deltaSeconds)
{
	RETV(m_state == MODULE_STATE::STOP, false);
	RETV(!m_udpSvr.IsConnect(), false);

	m_incTime += deltaSeconds;
	if (m_incTime < 0.033f)
		return true;

	BYTE buffer[1024];
	const int rcvLen = m_udpSvr.GetRecvData(buffer, sizeof(buffer));
	if (rcvLen > 0)
	{
		ParseUDPData(buffer, rcvLen);
	}

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

	m_incTime = 0;
	return true;
}


// UDP 정보를 분석해 심볼테이블에 저장한다.
void cUDPInput::ParseUDPData(const BYTE *buffer, const int bufferLen)
{
	int i = 0;
	int index = 0;
	const BYTE *pmem = buffer;
	for each (auto &field in m_protocolParser.m_fields)
	{
		if (index > bufferLen)
			break;

		script::sFieldData data;
		ZeroMemory(data.buff, sizeof(data));
		memcpy(data.buff, pmem, field.bytes);
		data.type = field.type;

		const string id = format("$%d", i + m_startIndex); // $1 ,$2, $3 ~
		const string oldId = format("@%d", i + m_startIndex); // @1, @2, @3 ~

		script::g_symbols[oldId] = script::g_symbols[id]; // 전 데이타를 저장한다. 
		script::g_symbols[id] = data;

		index += field.bytes;
		pmem += field.bytes;
		++i;
	}
}

