//
// 모션파형을 변조하는 기능을 한다.
//
#pragma once


class cMotionWaveModulator : public common::cConfig
{
public:
	cMotionWaveModulator();
	virtual ~cMotionWaveModulator();

	void Update(const float deltaSeconds, const float yaw, const float pitch, const float roll, const float heave);
	void GetFinal(OUT float &yaw, OUT float &pitch, OUT float &roll, OUT float &heave);
	void GetOriginal(OUT float &yaw, OUT float &pitch, OUT float &roll, OUT float &heave);

	virtual void InitDefault() override;
	virtual void UpdateParseData() override;


public:
	cModulator m_axis[4]; // yaw-pitch-roll-heave

	float m_incTime;
	float m_totalIncTime;
};

