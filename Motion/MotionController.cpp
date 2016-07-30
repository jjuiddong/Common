
#include "stdafx.h"
#include "MotionController.h"


cMotionController::cMotionController()
	: m_yaw(10000)
	, m_pitch(10000)
	, m_roll(10000)
	, m_heave(10000)
	, m_speed(0)
	, m_actuatorSpeed(1)
	, m_actuatorYawPower(1)
	, m_playTime(60)
{
}

cMotionController::~cMotionController()
{
}


bool cMotionController::ReadConfigFile(const string &fileName)
{
	return true;
}


bool cMotionController::WriteConfigFile(const string &fileName)
{
	return true;
}

