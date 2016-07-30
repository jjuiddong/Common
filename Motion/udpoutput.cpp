
#include "stdafx.h"
#include "udpoutput.h"

using namespace motion;


cUDPOutput::cUDPOutput() : cOutput(MOTION_MEDIA::UDP)
, m_sendIp("127.0.0.1")
, m_sendPort(0)
{
}

cUDPOutput::~cUDPOutput()
{
	for each (auto form in m_formats)
		delete form;
	m_formats.clear();
}


bool cUDPOutput::Init(const string &ip, const int port, const string &outputLog, const vector<string*> &formatScripts)
{
	m_sendIp = ip;
	m_sendPort = port;
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


bool cUDPOutput::Start()
{
	if (!m_udpClient.Init(m_sendIp, m_sendPort))
		return false;

	m_state = MODULE_STATE::START;

	// debug output log clear
	if (!m_outputLogFileName.empty())
		dbg::RemoveLog2(m_outputLogFileName.c_str());

	return true;
}


bool cUDPOutput::Stop()
{
	m_udpClient.Close();

	m_state = MODULE_STATE::STOP;
	return true;
}


bool cUDPOutput::Update(const float deltaSeconds)
{
	RETV(MODULE_STATE::STOP == m_state, false);
	RETV(!m_udpClient.IsConnect(), false);
	RETV(m_formats.empty(), false);
	RETV((int)m_formats.size() <= m_formatIdx, false);

	m_incTime += deltaSeconds;
	if (m_incTime < 0.033f)
		return true;

	BYTE buffer[128];
	const int len = m_formats[m_formatIdx]->Execute(buffer, sizeof(buffer));
	if (len > 0)
	{
		m_udpClient.SendData(buffer, len);

		if (!m_outputLogFileName.empty())
		{
			buffer[len] = NULL;
			dbg::Log2(m_outputLogFileName.c_str(), "%s \n", (char*)buffer);
		}
	}

	m_incTime = 0;
	return true;
}
