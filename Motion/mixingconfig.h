//
// mixing view 환경 설정 파일을 읽고, 저장한다.
//
//
#pragma once

enum
{
	INPUT_JOYSTICK = 0x01,
	INPUT_UDP = 0x02,
	INPUT_MOTIONWAVE = 0x04,
	INPUT_VAR = 0x08,
};


class cMixingConfig : public common::cConfig
{
public:
	cMixingConfig();
	virtual ~cMixingConfig();


protected:
	virtual void InitDefault() override;
	virtual void UpdateParseData() override;


public:
	int m_inputType;

	bool m_input1_enable;
	float m_rate1_all;
	float m_rate1_yaw;
	float m_rate1_pitch;
	float m_rate1_roll;
	float m_rate1_heave;
	float m_rate1_center_yaw;
	float m_rate1_center_pitch;
	float m_rate1_center_roll;
	float m_rate1_center_heave;

	bool m_input2_enable;
	float m_rate2_all;
	float m_rate2_yaw;
	float m_rate2_pitch;
	float m_rate2_roll;
	float m_rate2_heave;
	float m_rate2_center_yaw;
	float m_rate2_center_pitch;
	float m_rate2_center_roll;
	float m_rate2_center_heave;

	bool m_input3_enable;
	float m_rate3_all;
	float m_rate3_yaw;
	float m_rate3_pitch;
	float m_rate3_roll;
	float m_rate3_heave;
	float m_rate3_center_yaw;
	float m_rate3_center_pitch;
	float m_rate3_center_roll;
	float m_rate3_center_heave;

	bool m_input4_enable;
	float m_rate4_all;
	float m_rate4_pitch;
	float m_rate4_roll;
	float m_rate4_center_pitch;
	float m_rate4_center_roll;

	float m_bias_yaw;
	float m_bias_pitch;
	float m_bias_roll;
	float m_bias_heave;
};

