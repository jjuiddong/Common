
#include "stdafx.h"
#include "timer.h"

using namespace common;

cTimer::cTimer()
{
}

cTimer::~cTimer()
{
}


bool cTimer::Create()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		RETV2(1, false);

	//m_pcFreq = double(li.QuadPart) / 1000.0f; // milliseconds unit
	m_pcFreq = double(li.QuadPart); // seconds unit

	QueryPerformanceCounter(&li);
	m_counterStart = li.QuadPart;
	return true;
}


// return seconds 
double cTimer::GetSeconds()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - m_counterStart) / m_pcFreq;
}


// return milliseconds
double cTimer::GetMilliSeconds()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - m_counterStart) / (m_pcFreq / 1000.f); // milliseconds unit
}


// return seconds 
double cTimer::GetDeltaSeconds()
{
	const double t = GetSeconds();
	const double dt = t - m_prevCounter;
	m_prevCounter = t;
	return dt;
}
