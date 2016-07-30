
#include "stdafx.h"
#include "motion_export.h"

/*

int MotionInit(int hWnd, char *fileName)
{
	return motion::cController2::Get()->Init((HWND)hWnd, fileName);
}

int MotionInit2(char *fileName)
{
	return motion::cController2::Get()->Init(NULL, fileName);
}

void MotionUpdate(const float deltaSeconds)
{
	motion::cController2::Get()->Update(deltaSeconds);
}

void Clear()
{
	motion::cController2::Release();
	cvproc::imagematch::cMatchManager::Release();
}


int MotionStart()
{
	return motion::cController2::Get()->Start();
}

int MotionStop()
{
	return motion::cController2::Get()->Stop();
}

void MotionSetOutputFormat(int outputIndex, int formatIndex)
{
	motion::cController2::Get()->SetOutputFormat(outputIndex, formatIndex);
}

float MotionGetSymbolFloat(char *symbolName)
{
	return script::g_symbols[symbolName].fVal;
}

int MotionGetSymbolInt(char *symbolName)
{
	return script::g_symbols[symbolName].iVal;
}

void MotionSetSymbolFloat(char *symbolName, float val)
{
	script::g_symbols[symbolName].type = script::FIELD_TYPE::T_FLOAT;
	script::g_symbols[symbolName].fVal = val;
}

void MotionSetSymbolInt(char *symbolName, int val)
{
	script::g_symbols[symbolName].type = script::FIELD_TYPE::T_INT;
	script::g_symbols[symbolName].iVal = val;
}


int MatchReadScript(char *fileName, char *executeLabel)
{
	return cvproc::imagematch::cMatchManager::Get()->Init(fileName, executeLabel);	
}


void MatchCaptureAndDetect(char *result)
{
	string str = cvproc::imagematch::cMatchManager::Get()->CaptureAndDetect();
	strcpy(result, str.c_str());
}


void MatchCaptureDelay(int delayMilliSeconds)
{
	cvproc::imagematch::cMatchManager::Get()->CaptureDelay(delayMilliSeconds);
}
*/
