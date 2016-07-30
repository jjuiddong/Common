//
// VitCon 모션장비 초기화 와 동작시키는 것을 담당하는 클래스다.
//
// State Machine으로 구현되어 있다.
//
#pragma once


class cVitconMotionSim
{
public:
	cVitconMotionSim();
	virtual ~cVitconMotionSim();

	enum STATE {
		OFF,	// SUBSTATE_OFF일 때
		INIT,	// On() 일 때
		READY,  // SUBSTATE_READY일 때 
		PLAY,	// SUBSTATE_PLAYING일 때
		END,	// Off() 일 때
	};

	enum SUB_STATE {
		SUBSTATE_OFF,
		SUBSTATE_START_MOTION,
		SUBSTATE_INIT, // Servo On (5 seconds) 
		SUBSTATE_START, // Start (1 seconds)
		SUBSTATE_ORIGIN, // Origin Position (5 seconds)
		SUBSTATE_ORIGIN_STOP, // Origin Position (5 seconds) -> Stop
		SUBSTATE_STOP_READY, // Stop -> Ready
		SUBSTATE_READY, // Wait
		SUBSTATE_LOAD, // Load MotionWave
		SUBSTATE_PLAYING, // Play MotionWave
		SUBSTATE_END, // Origin Position(5 seconds)
		SUBSTATE_END_STOP, // Stop
		SUBSTATE_RESTART, // Origin Position(5 seconds) -> Start
		SUBSTATE_STOP_MOTION,
		SUBSTATE_STOP, // Servo Off
		SUBSTATE_DELAY,

		// VitCon 모션 시뮬레이터 실행 순서
		// INIT -> ORIGIN -> READY

		// 게임시작	
		// LOAD -> START -> PLAYING -> END

		// 게임종료, 대기
		// ORIGIN -> STOP -> READY

		// 재 게임
		// RESTART -> LOAD -> PLAYING -> END

		// 종료
		// END -> END_STOP
	};


public:
	bool Init(CBufferedSerial *serial);
	void Update(const float deltaSeconds);
	void On();
	void Ready();
	void Play();
	void Off();
	STATE GetState() const;
	SUB_STATE GetSubState() const;
	string GetStateStr() const;
	string GetSubStateStr() const;
	

protected:
	void State_Off(const float deltaSeconds);
	void State_Init(const float deltaSeconds);
	void State_Start(const float deltaSeconds);
	void State_Origin(const float deltaSeconds);
	void State_OriginStop(const float deltaSeconds);
	void State_StopReady(const float deltaSeconds);
	void State_Ready(const float deltaSeconds);
	void State_Load(const float deltaSeconds);
	void State_Playing(const float deltaSeconds);
	void State_End(const float deltaSeconds);
	void State_EndStop(const float deltaSeconds);
	void State_Restart(const float deltaSeconds);
	void State_Stop(const float deltaSeconds);
	void State_Delay(const float deltaSeconds);
	void Delay(const float delaySeconds, const SUB_STATE next);
	void SendMotionSimMessage(const int state);


protected:
	STATE m_state;
	SUB_STATE m_subState;
	CBufferedSerial *m_serial;

	float m_playTime; // seconds
	float m_lastUDPUpdateTime; // 가장 최근에 UDP 패킷을 받은 시간을 저장한다.

	bool m_changeInformationPulse; // game state가 바뀔 때만 true 가 된다.
	float m_delayTime;
	SUB_STATE m_delayNextState;
};


inline cVitconMotionSim::STATE cVitconMotionSim::GetState() const { return m_state;  }
inline cVitconMotionSim::SUB_STATE cVitconMotionSim::GetSubState() const { return m_subState; }
