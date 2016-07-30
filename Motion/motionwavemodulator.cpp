
#include "stdafx.h"
#include "motionwavemodulator.h"


cMotionWaveModulator::cMotionWaveModulator()
	: m_incTime(0)
	, m_totalIncTime(0)
{
	const int len = sizeof(m_axis) / sizeof(cModulator);
	for (int i = 0; i < len; ++i)
	{
		m_axis[i].Init();
	}
}

cMotionWaveModulator::~cMotionWaveModulator()
{
}


// 정보를 업데이트 한다.
void cMotionWaveModulator::Update(const float deltaSeconds,
	const float yaw, const float pitch, const float roll, const float heave)
{
	m_incTime += deltaSeconds;
	m_totalIncTime += deltaSeconds;
	if (m_incTime <= 0.001f)
		return;

	float value[4] = { yaw, pitch, roll, heave };
	const int axisLen = sizeof(m_axis) / sizeof(cModulator);
	for (int i = 0; i < axisLen; ++i)
		m_axis[i].Update(deltaSeconds, value[i]);

	m_incTime = 0;
}


// 최종적으로 계산된 결과 값을 리턴한다.
void cMotionWaveModulator::GetFinal(OUT float &yaw, OUT float &pitch, OUT float &roll, OUT float &heave)
{
	m_axis[0].GetFinal(yaw);
	m_axis[1].GetFinal(pitch);
	m_axis[2].GetFinal(roll);
	m_axis[3].GetFinal(heave);
}


void cMotionWaveModulator::GetOriginal(OUT float &yaw, OUT float &pitch, OUT float &roll, OUT float &heave)
{
	m_axis[0].GetOriginal(yaw);
	m_axis[1].GetOriginal(pitch);
	m_axis[2].GetOriginal(roll);
	m_axis[3].GetOriginal(heave);
}


// 변수 초기값 설정
void cMotionWaveModulator::InitDefault()
{
    m_incTime = 0;
	m_totalIncTime = 0;

	const int len = sizeof(m_axis) / sizeof(cModulator);
	for (int i = 0; i < len; ++i)
		m_axis[i].Init();
}


void cMotionWaveModulator::UpdateParseData()
{
	m_incTime = 0;

	// debug log
	if (script::g_isLog)
	{
		dbg::Log("motion wave modulation input string \n");
		for each (auto &kv in m_options)
			dbg::Log("\t%s = %s\n", kv.first.c_str(), kv.second.c_str());
		dbg::Log("\n");
	}

	m_axis[0].m_axis.a = GetFloat("yaw_c1");
	m_axis[0].m_axis.b = GetFloat("yaw_c2");
	m_axis[0].m_axis.c = GetFloat("yaw_c3");

	m_axis[1].m_axis.a = GetFloat("pitch_c1");
	m_axis[1].m_axis.b = GetFloat("pitch_c2");
	m_axis[1].m_axis.c = GetFloat("pitch_c3");

	m_axis[2].m_axis.a = GetFloat("roll_c1");
	m_axis[2].m_axis.b = GetFloat("roll_c2");
	m_axis[2].m_axis.c = GetFloat("roll_c3");

	m_axis[3].m_axis.a = GetFloat("heave_c1");
	m_axis[3].m_axis.b = GetFloat("heave_c2");
	m_axis[3].m_axis.c = GetFloat("heave_c3");

	string axis[4] = { "yaw", "pitch", "roll", "heave" };
	for (int i = 0; i < 4; ++i)
	{
		m_axis[i].m_axis.Kp = GetFloat(axis[i] + "_kp", 1.f);
		m_axis[i].m_axis.Ki = GetFloat(axis[i] + "_ki", 0.f);
		m_axis[i].m_axis.Kd = GetFloat(axis[i] + "_kd", 0.f);
		m_axis[i].m_axis.oldDiff = 0;
		m_axis[i].m_axis.incDiff = 0;

		m_axis[i].m_axis.recoverEnable = GetBool(axis[i] + "_recover_enable", true);
		m_axis[i].m_axis.recoverTarget = GetFloat(axis[i] + "_recover_target", 0);
		m_axis[i].m_axis.recoverProportion = GetFloat(axis[i] + "_recover_proportion", 0.01f);

		m_axis[i].m_axis.maxDifferenceEnable = GetBool(axis[i] + "_max_difference_enable", true);
		m_axis[i].m_axis.maxDifference = GetFloat(axis[i] + "_max_difference", MATH_PI);
		m_axis[i].m_axis.maxDifferenceProportion = GetFloat(axis[i] + "_max_difference_proportion", 0.1f);
		m_axis[i].m_axis.range = GetFloat(axis[i] + "_range", MATH_PI * 2.f);

		m_axis[i].m_axis.rangeEnable = GetBool(axis[i] + "_range_enable", false);
		m_axis[i].m_axis.rangeMax = GetFloat(axis[i] + "_range_max", FLT_MAX);
		m_axis[i].m_axis.rangeMin = GetFloat(axis[i] + "_range_min", FLT_MIN);

		m_axis[i].m_axis.isUseCalcValue = GetBool(axis[i] + "_use_x0_calcvalue", true);
	}
}


// 스플라인 곡선 리턴
// bool cMotionWaveModulator::GetSplineInterpolations(const float s, const float t, vector<Vector2> &yawOut, vector
// 	<Vector2> &pitchOut, vector<Vector2> &rollOut, vector<Vector2> &heaveOut)
// {
// 	yawOut.reserve(4);
// 	pitchOut.reserve(4);
// 	rollOut.reserve(4);
// 	heaveOut.reserve(4);
// 
// 	const bool ret1 = m_axis[0].m_spline.GetInterpolations(s, t, yawOut);
// 	const bool ret2 = m_axis[1].m_spline.GetInterpolations(s, t, pitchOut);
// 	const bool ret3 = m_axis[2].m_spline.GetInterpolations(s, t, rollOut);
// 	const bool ret4 = m_axis[3].m_spline.GetInterpolations(s, t, heaveOut);
// 
// 	return ret1 && ret2 && ret3 && ret4;
// }

