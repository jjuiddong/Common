#pragma once


class cModulator
{
public:
	cModulator();
	virtual ~cModulator();

	struct sAxisOption
	{
		// pid control
		float Kp; // p
		float Ki; // i
		float Kd; // d
		float oldDiff;
		float incDiff;
		//

		bool recoverEnable;
		float recoverTarget;
		float recoverProportion;

		bool maxDifferenceEnable;
		float maxDifference;
		float maxDifferenceProportion;
		float range;

		bool isPIClamp;
		bool isUseCalcValue;	// PID 계산시 x0 값을 현재 값을 사용할지, PID 계산된 후의 현재 값을 사용할지를 결정한다.

		bool rangeEnable;
		float rangeMax;
		float rangeMin;

		// ax^2 + bx + c
		float a;
		float b;
		float c;
		float value[3]; // original, pid calc value, final
	};


	void Init();
	void Update(const float deltaSeconds, const float x);
	void GetFinal(OUT float &out);
	void GetOriginal(OUT float &out);


protected:
	float GetDifference(const float x0, const float x1,
		const float proportionK, const float integralK, const float differenceK,
		const float range, const float maxDiff, const float maxDiffProportion);
	float GetManufactureValue(const float x0, const float x1,
		cModulator::sAxisOption &option);
	float GetNormalDifference(const float x00, const float x0, const float x1, const bool useCalcValue);
	float GetRecoverValue(const float x, const float target, const float proportion);


public:
	sAxisOption m_axis;
	bool m_firstLoop; // 처음 계산시 true

	// Spline계산을 위해 4포인트의 값을 저장한다.
	cSpline m_spline;

	// spline
	bool m_isSplineEnable;
	int m_splinePlotSamplingRate;
	int m_splineInterpolationRate;

	// motion wave
	int m_motionviewSplineSamplingRate;
	int m_motionviewSplineInterpolationRate;
	float m_motionviewTimeScaling;
	int m_motionviewStartDelay;

	float m_incTime;
	float m_totalIncTime;
};
