//
// 전역 환경설정 파일을 읽어서, cConfig 클래스의 멤버 변수에 저장한다.
//
#pragma once


struct sCameraConfig
{
	int cameraFrameDelay;
	int cameraPlayer1Index;
	int cameraPlayer2Index;

	bool showCamera;
	bool showScreen;
	bool showDetectPoint;
	bool showDebug;
	bool showCameraFrame;
	bool showCameraThreadFrame;

// 	bool stream_player1_enable;
// 	bool stream_player2_enable;
	int stream_player; // -1,0,1,
	int stream_server_port;
	bool stream_send_gray;
	bool stream_send_compressed;
	int streaming_send_quality;

	bool printMachineGunPlayer1XY;
	bool printMachineGunPlayer2XY;
	float pidControlP;
	float pidControlI;
	float pidControlD;

	bool monitor_mode; // 0:client, 1:server
	char monitor_ip[16];
	int monitor_port;

	char udpClientConnectIp[16];
	int udpClientConnectPort;
};



class cCamConfig
{
public:
	cCamConfig();
	virtual ~cCamConfig();

	bool Parse(const string &fileName);
	bool ParseStr(const string &str);
	bool Write(const string &fileName);


protected:
	void UpdateParseData();
	void InitDefault();
	bool GetBool(const string &key);
	float GetFloat(const string &key);
	int GetInt(const string &key);


public:
	map<string, string> m_options;

	int m_gameMode; // 0=game, 1=gun point check, 2=gun point display
	string m_projectName; // poject name

	sCameraConfig m_conf;

	string m_sharedMemName;
	bool m_printNetworkMessage;

	int m_cameraWidth;
	int m_cameraHeight;
	int m_cameraIndex;
	int m_screen_BinaryThreshold;
	int m_rotateSensitivity;

	// camera undistortion
	double m_cx;
	double m_cy;
	double m_fx;
	double m_fy;
	double m_k1;
	double m_k2;
	double m_k3;
	double m_p1;
	double m_p2;
	double m_skew_c;

	bool m_changeStreamingOption; // 독립적으로 쓰이는 변수이며, 특수용도로 쓰인다.


	// Windows
	cv::Rect m_screenRect;
	cv::Rect m_cameraRect;
	cv::Rect m_screenBinRect;
	cv::Rect m_screenRecogRect;
	cv::Rect m_skewScreenRect;
	cv::Rect m_matchScreenRect;
	cv::Rect m_detectPointBinaryScreenRect;

	bool m_screenEnable;
	bool m_cameraEnable;
	bool m_screenBinEnable;
	bool m_screenRecogEnable;
	bool m_skewScreenEnable;
	bool m_matchScreenEnable;
	bool m_detectPointBinaryScreenEnable;

	bool m_matchScreenDrawBox;

	int m_udpDevice1Port;
	int m_udpDevice2Port;
	int m_udpDevice3Port;
	int m_udpSleepMilliseconds;
	bool m_printUDPDevice1Message;
	bool m_printUDPDevice2Message;
	bool m_printUDPDevice3Message;

	string m_udpClientConnectIp;
	int m_udpClientConnectPort;
};

extern cCamConfig g_config;
