//
// Dirt3 레이싱 모션 시뮬레이터 용 VITCON 보드 Version.2 통신 클래스
//
// StateMachine 기반 보드 제어
//
#pragma once


class cVitconMotionSim2
{
public:
	cVitconMotionSim2();
	virtual ~cVitconMotionSim2();

	enum STATE {
		OFF,	// SUBSTATE_OFF일 때
		INIT,	// On() 일 때
		READY,  // SUBSTATE_READY일 때 
		READY_PLAY, // SUBSTATE_READY에서 SUBSTATE_PLAYING으로 넘어가는 상태
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
		// INIT -> SUBSTATE_STOP_READY -> READY

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
	bool Init(cSerialAsync *serial);
	void Update(const float deltaSeconds);
	void On();
	void Ready();
	void ReadyNoOrigin();
	bool Play();
	void Stop();
	void Off();

	STATE GetState() const;
	SUB_STATE GetSubState() const;
	string GetStateStr() const;
	string GetSubStateStr() const;
	float GetPlayTime() const;


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
	void SendMotionSimMessage2(const int state);


protected:
	STATE m_state;
	SUB_STATE m_subState;
	cSerialAsync *m_serial;

	float m_playTime; // seconds
	float m_delayTime;
	SUB_STATE m_delayNextState;
};


inline cVitconMotionSim2::STATE cVitconMotionSim2::GetState() const { return m_state; }
inline cVitconMotionSim2::SUB_STATE cVitconMotionSim2::GetSubState() const { return m_subState; }
inline float cVitconMotionSim2::GetPlayTime() const { return m_playTime;  }
