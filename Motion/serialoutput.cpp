
#include "stdafx.h"
#include "serialoutput.h"

using namespace motion;


cSerialOutput::cSerialOutput() 
	: cOutput(MOTION_MEDIA::SERIAL)
	, m_isAutoClose(true)
{
}

cSerialOutput::~cSerialOutput()
{
	for each (auto form in m_formats)
		delete form;
	m_formats.clear();
}


// bool cSerialOutput::Init(const int portNum, const int baudRate, const string &formatScript)
// {
// 	m_portNum = portNum;
// 	m_baudRate = baudRate;
// 
// 	if (!m_formatParser.Parse(formatScript))
// 		return false;
// 
// 	return true;
// }


bool cSerialOutput::Init(const int portNum, const int baudRate, const string &outputLog, const vector<string*> &formatScripts)
{
	m_portNum = portNum;
	m_baudRate = baudRate;
	m_outputLogFileName = outputLog;
	
	for each (auto scr in formatScripts)
	{
		cFormatParser *form = new cFormatParser();
		if (!form->Parse(*scr))
			return false;
		m_formats.push_back(form);
	}
	return true;
}


// 시리얼 출력 시작
bool cSerialOutput::Start()
{
	if (!m_serial.Open(m_portNum, m_baudRate))
	{
		if (m_serial.IsOpen())
		{ 
			dbg::Log("serial already opend \n");
		}
		else
		{
			dbg::Log("Serial Open Error!! \n");
			return false;
		}
	}

	m_state = MODULE_STATE::START;
	
	// serial port, baudrate 저장
	script::g_symbols["@output_serial_port"].type = script::FIELD_TYPE::T_INT;
	script::g_symbols["@output_serial_port"].iVal = m_portNum;
	script::g_symbols["@output_serial_baudrate"].type = script::FIELD_TYPE::T_INT;
	script::g_symbols["@output_serial_baudrate"].iVal = m_baudRate;

	// debug output log clear
	if (!m_outputLogFileName.empty())
		dbg::RemoveLog2(m_outputLogFileName.c_str());

	return true;
}


// 시리얼 출력 종료
bool cSerialOutput::Stop()
{
	// 모션장치 종료를 할 동안 시리얼통신을 해야하므로, 통신을 끊지 않는 경우도 있다.
	if (m_isAutoClose)
		m_serial.Close();

	m_state = MODULE_STATE::STOP;
	
	return true;
}


// 시리얼 출력 처리
bool cSerialOutput::Update(const float deltaSeconds)
{
	RETV(MODULE_STATE::STOP == m_state, false);
	RETV(!m_serial.IsOpen(), false);
	RETV(m_formats.empty(), false);
	RETV((int)m_formats.size() <= m_formatIdx, false);

	m_incTime += deltaSeconds;
	//if (m_incTime < 0.033f)
	//if (m_incTime < 0.039f)
	if (m_incTime < 0.049f)
		return true;

	BYTE buffer[128];
	const int len = m_formats[m_formatIdx]->Execute(buffer, sizeof(buffer));
	if (len > 0)
	{
		m_serial.SendData(buffer, len);

		if (!m_outputLogFileName.empty())
		{
			buffer[len] = NULL;
			dbg::Log2(m_outputLogFileName.c_str(), "%s \n", (char*)buffer);
		}
	}

	m_incTime = 0;
	return true;
}


// 즉시 정보를 송신한다.
void cSerialOutput::SendImmediate()
{
	RET(m_formats.empty());
	RET((int)m_formats.size() <= m_formatIdx);

	BYTE buffer[128];
	const int len = m_formats[m_formatIdx]->Execute(buffer, sizeof(buffer));
	if (len > 0)
		m_serial.SendData(buffer, len);
}


void cSerialOutput::SetFormat(const int index)
{ 
	cOutput::SetFormat(index); 
	dbg::Log("Motion SerialOutput SetFormat %d \n", index);
}

