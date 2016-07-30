
#include "stdafx.h"
#include "LEDBlinkCapture.h"


using namespace cvproc;


cLEDBlinkCapture::cLEDBlinkCapture() :
	m_delayMilliseconds(100)
	, m_oldT(0)
	, m_chCapture(false)
	, m_chToggle(false)
	, m_toggle(false)
{

}

cLEDBlinkCapture::~cLEDBlinkCapture()
{

}


// ÃÊ±âÈ­
bool cLEDBlinkCapture::Init(const int portNum, const int baudRate, const int delayMilliseconds)
{
	m_serial.Close();
	if (!m_serial.Open(portNum, baudRate))
 		return false;

	m_delayMilliseconds = delayMilliseconds;

	return true;
}


void cLEDBlinkCapture::Update(const cv::Mat &src)
{
	const int curT = timeGetTime();
	const int deltaT = curT - m_oldT;

	if (!m_chCapture && (deltaT > m_delayMilliseconds))
	{
		if (m_toggle)
			LEDOn();
		else
			LEDOff();

		char readBuff[256];
		m_serial.ReadData(readBuff, sizeof(readBuff));

		m_toggle = !m_toggle;
		m_chToggle = true;
		m_chCapture = true;
		m_oldT = curT;
	}

	if (m_chToggle && (curT - m_oldT > m_delayMilliseconds))
	{
		m_chToggle = false;
		m_chCapture = false;

		if (m_toggle)
			m_ledOff = src.clone();
		else
			m_ledOn = src.clone();
	}
}


void cLEDBlinkCapture::LEDOn()
{
	char buff[64];
	buff[0] = '$';
	buff[1] = 'M';
	buff[2] = '>';
	buff[3] = '0';
	buff[4] = '1';
	buff[5] = buff[3] ^ buff[4];
	m_serial.SendData(buff, sizeof(buff));
}


void cLEDBlinkCapture::LEDOff()
{
	char buff[64];
	buff[0] = '$';
	buff[1] = 'M';
	buff[2] = '>';
	buff[3] = '0';
	buff[4] = '0';
	buff[5] = buff[3] ^ buff[4];
	m_serial.SendData(buff, sizeof(buff));
}


//
void cLEDBlinkCapture::DetectIgnorePoint(const cv::Mat &src)
{


}
