
#include "stdafx.h"
#include "Controller.h"
//#include "SerialCommunication.h"



cController::cController() :
	m_globalSeconds(0)
{
	m_serialComm = new cSerialAsync();
	m_udpServer.m_sleepMillis = 1;
}


cController::~cController()
{
	SAFE_DELETE(m_serialComm);
}


// 큐브와 시리얼통신으로 연결을 시도한다.
bool cController::ConnectSerial(const int portNum, const int baudRate)
{
	RETV(!m_serialComm, false);
	return m_serialComm->Open(portNum, baudRate);
}


// 시리얼 통신을 종료한다.
void cController::CloseSerial()
{
	RET(!m_serialComm);
	m_serialComm->Close();
}


void cController::Update(const float deltaSeconds)
{
	RET(!m_serialComm);

	m_globalSeconds += deltaSeconds;

	//m_serialComm->ProcessSerialCommunicatoin(deltaSeconds);	

	BYTE buffer[1024];
	const int bufferLen = m_udpServer.GetRecvData(buffer, sizeof(buffer));
	if (bufferLen > 0)
	{
		if (bufferLen < sizeof(buffer) / sizeof(char))
			buffer[bufferLen] = NULL;
		NotifyUDPObserver((char*)buffer, bufferLen);
	}

	NotifyUpdateObserver(deltaSeconds);
}
