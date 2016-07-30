
#include "stdafx.h"
#include "simplemodulator.h"


cSimpleModulator::cSimpleModulator()
{
}

cSimpleModulator::~cSimpleModulator()
{
}


void cSimpleModulator::InitDefault()
{
	m_incTime = 0;
	m_totalIncTime = 0;

	m_isSplineEnable = false;

	m_splinePlotSamplingRate = 10;
	m_splineInterpolationRate = 10;

	m_motionviewSplineSamplingRate = 2;
	m_motionviewSplineInterpolationRate = 2;
	m_motionviewTimeScaling = 1;
	m_motionviewStartDelay = 0;

	m_axis.a = 0;
	m_axis.b = 1;
	m_axis.c = 0;

	m_axis.Kp = 1;
	m_axis.Ki = 0;
	m_axis.Kd = 0;
	m_axis.oldDiff = 0;
	m_axis.incDiff = 0;

	m_axis.recoverEnable = false;
	m_axis.recoverTarget = false;
	m_axis.recoverProportion = 0.01f;

	m_axis.maxDifferenceEnable = false;
	m_axis.maxDifference = MATH_PI;
	m_axis.maxDifferenceProportion = 0.1f;
	m_axis.range = MATH_PI * 2.f;

	m_axis.isPIClamp = true;
}


void cSimpleModulator::UpdateParseData()
{
	// debug log
	if (script::g_isLog)
	{
		dbg::Log("simple modulation input string \n");
		for each (auto &kv in m_options)
			dbg::Log("\t%s = %s\n", kv.first.c_str(), kv.second.c_str());
		dbg::Log("\n");
	}

	m_axis.a = GetFloat("c1");
	m_axis.b = GetFloat("c2");
	m_axis.c = GetFloat("c3");

	m_axis.Kp = GetFloat("kp", 1.f);
	m_axis.Ki = GetFloat("ki", 0.f);
	m_axis.Kd = GetFloat("kd", 0.f);
	m_axis.oldDiff = 0;
	m_axis.incDiff = 0;

	m_axis.recoverEnable = GetBool("recover_enable", true);
	m_axis.recoverTarget = GetFloat("recover_target", 0);
	m_axis.recoverProportion = GetFloat("recover_proportion", 0.01f);

	m_axis.maxDifferenceEnable = GetBool("max_difference_enable", true);
	m_axis.maxDifference = GetFloat("max_difference", MATH_PI);
	m_axis.maxDifferenceProportion = GetFloat("max_difference_proportion", 0.1f);
	m_axis.range = GetFloat("range", MATH_PI * 2.f);

	m_axis.isPIClamp = GetBool("piclamp_enable", true);
	m_axis.isUseCalcValue = GetBool("use_x0_calcvalue", true);

	m_axis.rangeEnable = GetBool("range_enable", false);
	m_axis.rangeMax = GetFloat("max_range", 100000000);
	m_axis.rangeMin = GetFloat("min_range", 0);
}
