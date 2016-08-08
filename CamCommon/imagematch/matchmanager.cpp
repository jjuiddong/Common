
#include "stdafx.h"
#include "matchmanager.h"
#include "matchprocessor.h"

using namespace cvproc;
using namespace cvproc::imagematch;
using namespace cv;

cMatchManager::cMatchManager() 
	: m_delayCapture(100)
{
}

cMatchManager::~cMatchManager()
{
}


bool cMatchManager::Init(const string &filename, const string &executeLabel)
{
	dbg::RemoveLog();
	dbg::RemoveErrLog();

	m_executeLabel = executeLabel;
	return m_matchScript.Read(filename);
}


string cMatchManager::CaptureAndDetect()
{
	cScreenCapture capture;
	capture.m_prtScrSleepTime = m_delayCapture;
	Mat img = capture.ScreenCapture(true);
	
	cMatchResult result;
	result.Init(&m_matchScript, img, "@input", 0,
		(sParseTree*)m_matchScript.FindTreeLabel(m_executeLabel), true, true);

	cMatchProcessor::Get()->Match(result);

	return result.m_resultStr;
}


void cMatchManager::CaptureDelay(const int delayMilliSeconds)
{
	m_delayCapture = delayMilliSeconds;
}
