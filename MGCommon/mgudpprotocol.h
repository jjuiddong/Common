//
// UDP로 전송되는 프로토콜을 정리함
//
//
#pragma once




//--------------------------------------------------------------------------------------------------------------
// 머신건 매인보드에서 보내는 UDP 패킷 프로토콜
// Stand Version


struct sMGDevicePacket
{
	char header; // $
	char comma1; // ,
	char deviceNumber; // 9
	char zero1; // 0
	char comma2; // ,

	char player1Fire;			// 0/1
	char player1FireEvent;		// 0/1
	char player1Reload;			// 0/1
	char player1Start;			// 0/1
	char player1UpMotor;		// 0/1
	char player1DownMotor;		// 0/1
	char player1UpSensor;		// 0/1
	char player1DownSensor;		// 0/1
	char player1EmergencySwitch;// 0/1
	char coin;
	char space1;

	char player2Fire;			// 0/1
	char player2FireEvent;		// 0/1
	char player2Reload;			// 0/1
	char player2Start;			// 0/1
	char player2UpMotor;		// 0/1
	char player2DownMotor;		// 0/1
	char player2UpSensor;		// 0/1
	char player2DownSensor;		// 0/1
	char player2EmergencySwitch;// 0/1
	char space3;
	char space4;

	char comma3;
	char at;

	char cr[2];
};



//--------------------------------------------------------------------------------------------------------------
// 머신건 매니저에서 게임 클라이언트에게 보내는 패킷 프로토콜

struct SMGCameraData
{
	float x1, y1, x2, y2; // 총이 가르키는 위치 0 ~ 1 사이 값. 화면의 왼쪽 아래가 {0,0}
	int fire1; // 플레어1 격발, 1:격발, 0:Nothing~
	int fire2; // 플레어2 격발, 1:격발, 0:Nothing~
	int reload1; // 플레어1 리로드, 1:리로드, 0:Nothing~
	int reload2; // 플레어2 리로드, 1:리로드, 0:Nothing~
	int start1; // 플레어1 스타트버튼 On/Off, 1:On, 0:Off
	int start2; // 플레어2 스타트버튼 On/Off, 1:On, 0:Off
	int credit; // 게임 플레이 할 수 있는 회수
	int coinCount; // 여분의 동전 개수
	int coinPerGame; // 한 게임당 동전 개수
};

