
#include "stdafx.h"
#include "vitconmotionsim3.h"


cVitconMotionSim3::cVitconMotionSim3() :
	m_state(OFF)
	, m_serial(NULL)
	, m_subState(SUBSTATE_OFF)
	, m_playTime(0)
	, m_delayTime(0)
{
}

cVitconMotionSim3::~cVitconMotionSim3()
{
}


bool cVitconMotionSim3::Init(cSerialAsync *serial)
{
	m_serial = serial;

	return true;
}


void cVitconMotionSim3::Update(const float deltaSeconds)
{

	switch (m_subState)
	{
	case SUBSTATE_OFF: State_Off(deltaSeconds); break;
	case SUBSTATE_INIT: State_Init(deltaSeconds); break;
	case SUBSTATE_START: State_Start(deltaSeconds); break;
	case SUBSTATE_ORIGIN: State_Origin(deltaSeconds); break;
	case SUBSTATE_ORIGIN_STOP: State_OriginStop(deltaSeconds); break;
	case SUBSTATE_STOP_READY: State_StopReady(deltaSeconds); break;
	case SUBSTATE_READY: State_Ready(deltaSeconds); break;
	case SUBSTATE_LOAD: State_Load(deltaSeconds); break;
	case SUBSTATE_PLAYING: State_Playing(deltaSeconds); break;
	case SUBSTATE_END: State_End(deltaSeconds); break;
	case SUBSTATE_END_STOP: State_EndStop(deltaSeconds); break;
	case SUBSTATE_RESTART: State_Restart(deltaSeconds); break;
	case SUBSTATE_STOP: State_Stop(deltaSeconds); break;
	case SUBSTATE_DELAY: State_Delay(deltaSeconds); break;

	case SUBSTATE_START_MOTION: m_subState = SUBSTATE_INIT; break;
	case SUBSTATE_STOP_MOTION: m_subState = SUBSTATE_END; break;
	default:
		break;
	}

}


// 모션시뮬레이터를 켜고, ServoOn -> Origin -> Stop 상태로 만든다.
// INIT -> ORIGIN -> READY
// SUBSTATE_START_MOTION -> SUBSTATE_INIT -> SUBSTATE_ORIGIN -> SUBSTATE_READY
void cVitconMotionSim3::On()
{
	m_state = INIT;
	m_subState = SUBSTATE_START_MOTION;
}


// 플레이를 종료하고, Origin으로 돌아간후, Stop 상태에서 대기한다.
// STOP -> READY
void cVitconMotionSim3::Ready()
{
	m_state = READY_STOP;
	m_subState = SUBSTATE_ORIGIN_STOP;
	m_playTime = 0;
}


// 플레이를 종료하고, Origin없이, Stop 상태에서 대기한다.
// STOP -> READY
void cVitconMotionSim3::ReadyNoOrigin()
{
	m_playTime = 0;
	Delay(0.1f, SUBSTATE_STOP_READY);
}


// 모션시뮬레이터를 START 상태로 만든다.
bool cVitconMotionSim3::Play()
{
	switch (m_state)
	{
	case cVitconMotionSim3::OFF:
	case cVitconMotionSim3::INIT:
	case cVitconMotionSim3::END:
		return false; // 아무일 없음.

	case cVitconMotionSim3::READY_PLAY:
		return true;

	case cVitconMotionSim3::READY_STOP:
		m_state = READY_PLAY;
		m_subState = SUBSTATE_START; // 게임을 Stop 중인 상태라면, 멈추고, 재시작한다.
		break;

	case cVitconMotionSim3::PLAY:
		if (SUBSTATE_DELAY == m_subState)
			m_subState = SUBSTATE_START; // 게임을 Stop 중인 상태라면, 멈추고, 재시작한다.
		return true;

	case cVitconMotionSim3::READY:
		m_state = READY_PLAY;
		m_subState = SUBSTATE_START; // 모션장치를 동작시킨다. (Start 상태로 만든다.)
		return true;

	default:
		return false;
	}

	return false;
}


// 모션장치를 Stop 상태로 만든다.
void cVitconMotionSim3::Stop()
{
	switch (m_state)
	{
	case cVitconMotionSim3::OFF:
	case cVitconMotionSim3::INIT:
	case cVitconMotionSim3::END:
	case cVitconMotionSim3::READY:
	case cVitconMotionSim3::READY_STOP:
		break; // 아무일 없음.

	case cVitconMotionSim3::READY_PLAY:
	case cVitconMotionSim3::PLAY:
		Ready(); // 모션장치를 Stop 시킨다.
		break;
	}
}


// 모션시뮬레이터를 종료하고 끈다. Stop (3 seconds delay) -> ServoOff
// END -> END_STOP
void cVitconMotionSim3::Off()
{
	m_state = END;
	m_subState = SUBSTATE_STOP_MOTION;
}


void cVitconMotionSim3::State_Off(const float deltaSeconds)
{
	m_state = OFF;
}


// Init State
// 1. 게임 환경설정 파일을 읽는다.
// 2. 모든 뷰를 초기화 하고, Start 상태로 동작하게 한다.
void cVitconMotionSim3::State_Init(const float deltaSeconds)
{
	SendMotionSimMessage2(VITCON_SER::SERVOON);
	Delay(1, SUBSTATE_STOP_READY); // 1초 후, Stop 상태에서 대기하게한다.
}


void cVitconMotionSim3::State_Start(const float deltaSeconds)
{
	m_state = READY_PLAY;
	SendMotionSimMessage2(VITCON_SER::START);
	Delay(0.1f, SUBSTATE_PLAYING);
}


// Origin State
void cVitconMotionSim3::State_Origin(const float deltaSeconds)
{
	Delay(0, SUBSTATE_READY);
}


void cVitconMotionSim3::State_OriginStop(const float deltaSeconds)
{
	Delay(0.5f, SUBSTATE_STOP_READY);
}


void cVitconMotionSim3::State_StopReady(const float deltaSeconds)
{
	SendMotionSimMessage2(VITCON_SER::STOP);
	Delay(3, SUBSTATE_READY);
}


void cVitconMotionSim3::State_Ready(const float deltaSeconds)
{
	// 아무일 없이 대기
	m_state = READY;
}


// Load State
void cVitconMotionSim3::State_Load(const float deltaSeconds)
{
	Delay(0.1f, SUBSTATE_START);
}


// Playing State
void cVitconMotionSim3::State_Playing(const float deltaSeconds)
{
	m_state = PLAY;
	m_playTime += deltaSeconds;
}


// End State
void cVitconMotionSim3::State_End(const float deltaSeconds)
{
	Delay(1, SUBSTATE_END_STOP);
}


// EndStop State
void cVitconMotionSim3::State_EndStop(const float deltaSeconds)
{
	SendMotionSimMessage2(VITCON_SER::STOP);
	Delay(3, SUBSTATE_STOP);
}


// Restart State
void cVitconMotionSim3::State_Restart(const float deltaSeconds)
{
}


// Stop State
void cVitconMotionSim3::State_Stop(const float deltaSeconds)
{
	SendMotionSimMessage2(VITCON_SER::SERVOOFF);
	Delay(1, SUBSTATE_OFF);
}


// Delay State
void cVitconMotionSim3::State_Delay(const float deltaSeconds)
{
	m_delayTime -= deltaSeconds;

	if (m_delayTime < 0)
	{
		m_subState = m_delayNextState;
	}
}


// Delay State
void cVitconMotionSim3::Delay(const float delaySeconds, const SUB_STATE next)
{
	if (next == SUBSTATE_DELAY)
		return;

	m_subState = SUBSTATE_DELAY;
	m_delayTime = delaySeconds;
	m_delayNextState = next;
}


string cVitconMotionSim3::GetStateStr() const
{
	string str;
	switch (m_state)
	{
	case cVitconMotionSim3::OFF: str = "Off"; break;
	case cVitconMotionSim3::INIT: str = "Init"; break;
	case cVitconMotionSim3::READY: str = "Ready"; break;
	case cVitconMotionSim3::READY_PLAY: str = "Ready Play"; break;
	case cVitconMotionSim3::PLAY: str = "Play"; break;
	case cVitconMotionSim3::READY_STOP: str = "ReadyStop"; break;
	case cVitconMotionSim3::END: str = "End"; break;
	default: str = "None"; break;
	}
	return str;
}


string cVitconMotionSim3::GetSubStateStr() const
{
	string str;
	switch (m_subState)
	{
	case cVitconMotionSim3::SUBSTATE_OFF: str = "Off"; break;
	case cVitconMotionSim3::SUBSTATE_START_MOTION: str = "Start_Motion"; break;
	case cVitconMotionSim3::SUBSTATE_INIT: str = "Init"; break;
	case cVitconMotionSim3::SUBSTATE_START: str = "Start"; break;
	case cVitconMotionSim3::SUBSTATE_ORIGIN: str = "Origin"; break;
	case cVitconMotionSim3::SUBSTATE_ORIGIN_STOP: str = "Origin_Stop"; break;
	case cVitconMotionSim3::SUBSTATE_STOP_READY: str = "Stop_Ready"; break;
	case cVitconMotionSim3::SUBSTATE_READY: str = "Ready"; break;
	case cVitconMotionSim3::SUBSTATE_LOAD: str = "Load"; break;
	case cVitconMotionSim3::SUBSTATE_PLAYING: str = "Playing"; break;
	case cVitconMotionSim3::SUBSTATE_END: str = "End"; break;
	case cVitconMotionSim3::SUBSTATE_END_STOP: str = "End_Stop"; break;
	case cVitconMotionSim3::SUBSTATE_RESTART: str = "Restart"; break;
	case cVitconMotionSim3::SUBSTATE_STOP_MOTION: str = "Stop_Motion"; break;
	case cVitconMotionSim3::SUBSTATE_STOP: str = "Stop"; break;
	case cVitconMotionSim3::SUBSTATE_DELAY: str = common::format("Delay %1.1f", m_delayTime); break;
	default: str = "None"; break;
	}
	return str;
}


// Dirt3 motion simulator 용 프로토콜
void cVitconMotionSim3::SendMotionSimMessage2(const int state)
{
	RET(!m_serial);

	const int out_pitch = 10000;
	const int out_roll = 10000;
	const int out_yaw = 10000;
	const int out_sway = 10000;
	const int out_surge = 10000;
	const int out_heave = 10000;
	const int spareSpeed = 0;
	const int out_switch = state;

	// 	const string out = common::format(
	// 		"A%3d%3d%3d%3d%3d%3d%dZ", out_roll, out_pitch, out_yaw,
	// 		out_sway, out_surge, out_heave, out_switch);
	string out = common::format(
		"A%5d%5d%5d%5d%5d%5d%3d%3d%3d%3d%3d%3d%1dZ", out_roll, out_pitch, out_yaw,
		out_sway, out_surge, out_heave,
		spareSpeed, spareSpeed, spareSpeed, spareSpeed, spareSpeed, spareSpeed,
		out_switch);
	common::replaceAll(out, " ", "0"); // SpaceCharToZeroChar

	for (int i = 0; i < 5; ++i)
	{
		Sleep(50);
		m_serial->SendData((BYTE*)out.c_str(), out.size());
	}
	Sleep(50);
}
