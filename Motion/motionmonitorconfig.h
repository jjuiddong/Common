#pragma once


class cMotionMonitorConfig
{
public:
	cMotionMonitorConfig();
	virtual ~cMotionMonitorConfig();

	bool ReadConfigFile(const string &fileName);
	bool WriteConfigFile(const string &fileName);


public:
	string m_fileName;

	string m_mode; // machinegun_stand, machinegun, dirt3_release, dirt3, joystick, mwav

	int m_udpPort;
	string m_udpPlotCommand;
	string m_udpModCommand;
	string m_udpPlotInputCmd;
	string m_mwaveModCommand;
	string m_joystickModCommand;
	string m_mixingModCommand;
	int m_motionOutputComPort;
	int m_motionOutputBaudRate;

	string m_udpProtocolCommand;
	string m_mixingScript;
	string m_rollCommand;
	string m_pitchCommand;
	string m_yawCommand;
	string m_heaveCommand;
	int m_udpParsePort;

	string m_plotViewPlotCmd;
	string m_plotViewPlotInputCmd;

	float m_dirt3ViewPlayTime;
	int m_dirt3ViewAxisType;		// 0=4axis, 1=3axis
	float m_dirt3ViewActuatorPower;			// 0 ~ 1
	float m_dirt3ViewActuatorYawPower;	// 0 ~ 1
	float m_dirt3ViewActuatorSpeed;			// 0 ~ 1
	bool m_dirt3ViewAutoStart;

	string m_varModViewInputVar;
	string m_varModViewOutputVar;
	string m_varModViewScript;

	string m_varModViewInputVar2;
	string m_varModViewOutputVar2;
	string m_varModViewScript2;

	string m_varModViewInputVar3;
	string m_varModViewOutputVar3;
	string m_varModViewScript3;

	string m_varModViewInputVar4;
	string m_varModViewOutputVar4;
	string m_varModViewScript4;
};

