
#include "stdafx.h"
#include "modulator.h"


cModulator::cModulator()
{
	Init();
}

cModulator::~cModulator()
{
}


// 변화값을 리턴한다.
// 범위 값을 넘어가서 반전 되는 경우, range, maxDiff 값을 이용해서 처리한다.
// x0 : 그 전 정보
// x1 : 현재 정보
// proportion : PID, P
// range : x0,x1 range
// maxDiff : 최대 변화 값
// maxDiffProportion : 최대 변화값을 넘었을 때 proportion 값 정의
// x2Proportion : 변화값의 제곱의 비율 값
float cModulator::GetDifference(const float x0, const float x1,
	const float proportionK, const float integralK, const float differenceK,
	const float range, const float maxDiff, const float maxDiffProportion)
{
	float diff = (x1 - x0) * proportionK;

	if (abs(x1 - x0) > maxDiff)
	{
		if (x1 * x0 < 0)
		{// 극성이 다를 때.. 즉 limit 값을 넘겼을 때..
			// limit 를 넘긴 값을 보정한다.

			float newDiff = range - abs(x1 - x0);
			if (x1 > 0)
				newDiff = -newDiff;

			diff = newDiff * proportionK;
		}
		else
		{// 같은 극성일 때.. 큰 변화가 일어 났을 때..

			// proportion을 한 번더 적용한다.
			diff = diff * maxDiffProportion;
		}
	}

	return diff;
}


float cModulator::GetManufactureValue(const float x0, const float x1,
	cModulator::sAxisOption &option)
{
	float diff = (x1 - x0);
	if (abs(x1 - x0) > option.maxDifference)
	{
		if (x1 * x0 < 0)
		{// 극성이 다를 때.. 즉 limit 값을 넘겼을 때..
			// limit 를 넘긴 값을 보정한다.

			float newDiff = option.range - abs(x1 - x0);
			if (x1 > 0)
				newDiff = -newDiff;

			diff = newDiff * option.Kp;
		}
	}

	float ndiff = diff - option.oldDiff;
	float mv = option.Kp * diff + option.Kd * ndiff + option.Ki * option.incDiff;
	option.oldDiff = diff;
	option.incDiff += diff;

	return mv;
}


// 변화값을 리턴한다.
//diff = GetNormalDifference(m_axis.value[0], m_axis.value[1], x);
// x00 : prev value
// x0 : calc prev value
// x1 : current value
float cModulator::GetNormalDifference(const float x00, const float x0, const float x1, const bool useCalcValue)
{
// 	float diff = (x1 - x0) * proportion;
// 	return diff;

	const float c0 = (useCalcValue) ? x0 : x00;
	const float diff = x1 - c0;
	const float ndiff = (x1 - c0) - m_axis.oldDiff;
	const float mv = (m_axis.Kp * diff) + (m_axis.Kd * ndiff) + (m_axis.Ki * m_axis.incDiff);
	m_axis.oldDiff = diff;
	m_axis.incDiff += diff;

	return mv;
}


// x에서 target으로 복귀하는 정도를 리턴한다.
float cModulator::GetRecoverValue(const float x, const float target, const float proportion)
{
	return (target - x) * proportion;
}


void cModulator::Init()
{
	m_firstLoop = true;
	m_incTime = 0;
	m_totalIncTime = 0;

	m_axis.recoverEnable = true;
	m_axis.maxDifferenceEnable = true;
	m_axis.Kp = 1.f;
	m_axis.Ki = 0;
	m_axis.Kd = 0;
	m_axis.oldDiff = 0;
	m_axis.incDiff = 0;
	m_axis.recoverTarget = 0;
	m_axis.recoverProportion = 0.01f;
	m_axis.maxDifference = MATH_PI;
	m_axis.range = MATH_PI * 2.f;
	
	m_axis.isPIClamp = true;
	m_axis.isUseCalcValue = true;

	m_axis.rangeEnable = false;
	m_axis.rangeMax = FLT_MAX;
	m_axis.rangeMin = FLT_MIN;

	m_axis.a = 0;
	m_axis.b = 1;
	m_axis.c = 0;
	ZeroMemory(m_axis.value, sizeof(m_axis.value));
}


void cModulator::Update(const float deltaSeconds, const float x)
{
	m_incTime += deltaSeconds;
	m_totalIncTime += deltaSeconds;
	if (m_incTime <= 0.001f)
		return;

	// 처음 계산시, 현재 값으로 초기화 한다.
	if (m_firstLoop)
	{
		m_axis.value[0] = x;
		m_firstLoop = false;
	}

	//---------------------------------------------------------------------------------------
	// PID
	float diff = 0;
	if (m_axis.maxDifferenceEnable)
	{
		diff = GetManufactureValue(m_axis.value[0], x, m_axis);
		m_axis.value[1] += diff;
		if (m_axis.isPIClamp)
			m_axis.value[1] = common::clamp(-MATH_PI, MATH_PI, m_axis.value[1]);
	}
	else
	{
		diff = GetNormalDifference(m_axis.value[0], m_axis.value[1], x, m_axis.isUseCalcValue);
		m_axis.value[1] += diff;
	}

	m_axis.value[0] = x;


	//---------------------------------------------------------------------------------------
	// Limit
	if (m_axis.rangeEnable)
	{
		if (m_axis.value[1] > m_axis.rangeMax)
			m_axis.value[1] = m_axis.rangeMax;
		if (m_axis.value[1] < m_axis.rangeMin)
			m_axis.value[1] = m_axis.rangeMin;
	}

	//---------------------------------------------------------------------------------------
	// Recovery
	if (m_axis.recoverEnable)
	{
		m_axis.value[1] += GetRecoverValue(m_axis.value[1], m_axis.recoverTarget, 
			m_axis.recoverProportion);
	}

	//---------------------------------------------------------------------------------------
	// Scaling
	const float v = m_axis.value[1];
	m_axis.value[2] = (v * v * m_axis.a) + (v * m_axis.b) + m_axis.c;

	m_incTime = 0;

}


void cModulator::GetFinal(OUT float &out)
{
	out = m_axis.value[_countof(m_axis.value) - 1];
}


void cModulator::GetOriginal(OUT float &out)
{
	out = m_axis.value[0];
}
