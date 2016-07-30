//
//  모션 제어 계산을 한다.
//
#pragma once


class cMotionController : public common::cSingleton<cMotionController>
{
public:
	cMotionController();
	virtual ~cMotionController();

	bool ReadConfigFile(const string &fileName);
	bool WriteConfigFile(const string &fileName);

	float GetActuatorSpeed() const;
	void SetActuatorSpeed(const float speed);
	float GetActuatorYawPower() const;
	void SetActuatorYawPower(const float power);
	float GetPlayTime() const;
	void SetPlayTime(const float t);


public:
	// 최종적으로 계산된 값.
	float m_yaw;
	float m_pitch;
	float m_roll;
	float m_heave;
	float m_speed;

	
	// Joystick으로부터 받아서 계산된 값
	cMotionWaveModulator m_joystickMod;

	// UDP로 부터 받아서 계산된 값
	cMotionWaveModulator m_udpMod;

	// MotionWave로부터 받아서 계산된 값.
	cMotionWaveModulator m_mwavMod;

	// Pitch 축 연산
	cSimpleModulator m_varModulator1;

	// Roll 축 연산
	cSimpleModulator m_varModulator2;

	// Speed 연산
	cSimpleModulator m_varModulator3;

	// Norm 연산
	cSimpleModulator m_varModulator4;

	cMotionMonitorConfig m_config;

	float m_actuatorYawPower; // 0~1 사이 값
	float m_playTime; // 한 게임당 플레이 할 수 있는 시간. seconds 단위, default : 60
	float m_actuatorSpeed; // 0~1 사이 값

	string m_loginId;
};


inline float cMotionController::GetActuatorSpeed() const { return m_actuatorSpeed; }
inline void cMotionController::SetActuatorSpeed(const float speed) { m_actuatorSpeed = speed; }
inline float cMotionController::GetPlayTime() const { return m_playTime; }
inline void cMotionController::SetPlayTime(const float t) { m_playTime = t; }
inline float cMotionController::GetActuatorYawPower() const { return m_actuatorYawPower; }
inline void cMotionController::SetActuatorYawPower(const float power) { m_actuatorYawPower = power; }
