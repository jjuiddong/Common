
#include "stdafx.h"
#include "motionmonitorconfig.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>



cMotionMonitorConfig::cMotionMonitorConfig()
	: m_mode("Normal")
{
}

cMotionMonitorConfig::~cMotionMonitorConfig()
{
}


// 환경파일을 읽는다.
bool cMotionMonitorConfig::ReadConfigFile(const string &fileName)
{
	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		boost::property_tree::read_json(fileName, props);

		m_fileName = fileName;

		m_mode = props.get<string>("MODE", "Normal");
		m_udpPort = props.get<int>("UDP_PORT", 8888);
		m_udpPlotCommand = props.get<string>("UDP_PLOT_COMMAND", "");
		m_udpPlotInputCmd = props.get<string>("UDP_PLOTINPUT_COMMAND", "");
		m_udpModCommand = props.get<string>("UDP_MOD_COMMAND", "");
		m_mwaveModCommand = props.get<string>("MWAVE_MOD_COMMAND", "");
		m_joystickModCommand = props.get<string>("JOYSTICK_MOD_COMMAND", "");
		m_mixingModCommand = props.get<string>("MIXING_COMMAND", "");
		m_motionOutputComPort = props.get<int>("MOTION_OUTPUT_COMPORT", 0);
		m_motionOutputBaudRate = props.get<int>("MOTION_OUTPUT_BAUDRATE", 0);

		m_udpParsePort = props.get<int>("UDP_PARSE_PORT", 8888);
		m_udpProtocolCommand = props.get<string>("UDP_PROTOCOL_COMMAND", "");
		m_mixingScript = props.get<string>("MIXING_SCRIPT", "");
		m_rollCommand = props.get<string>("UDP_INPUT_ROLL_COMMAND", "");
		m_pitchCommand = props.get<string>("UDP_INPUT_PITCH_COMMAND", "");
		m_yawCommand = props.get<string>("UDP_INPUT_YAW_COMMAND", "");
		m_heaveCommand = props.get<string>("UDP_INPUT_HEAVE_COMMAND", "");

		m_plotViewPlotCmd = props.get<string>("PLOTVIEW_PLOT_COMMAND", "");
		m_plotViewPlotInputCmd = props.get<string>("PLOTVIEW_PLOTINPUT_COMMAND", "");

		m_dirt3ViewPlayTime = props.get<float>("DIRT3VIEW_PLAYTIME", 60.f);
		m_dirt3ViewAxisType = props.get<int>("DIRT3VIEW_AXISTYPE", 0);
		m_dirt3ViewActuatorPower = props.get<float>("DIRT3VIEW_ACTUATOR_RANGE", 1.f);
		m_dirt3ViewActuatorYawPower = props.get<float>("DIRT3VIEW_ACTUATOR_YAW_POWER", 1.f);
		m_dirt3ViewActuatorSpeed = props.get<float>("DIRT3VIEW_ACTUATOR_SPEED", 1.f);
		m_dirt3ViewAutoStart = props.get<bool>("DIRT3VIEW_AUTOSTART", false);

		m_varModViewInputVar = props.get<string>("VARMODVIEW_INPUT_VAR", "");
		m_varModViewOutputVar = props.get<string>("VARMODVIEW_OUTPUT_VAR", "");
		m_varModViewScript = props.get<string>("VARMODVIEW_SCRIPT", "");

		m_varModViewInputVar2 = props.get<string>("VARMODVIEW_INPUT_VAR2", "");
		m_varModViewOutputVar2 = props.get<string>("VARMODVIEW_OUTPUT_VAR2", "");
		m_varModViewScript2 = props.get<string>("VARMODVIEW_SCRIPT2", "");

		m_varModViewInputVar3 = props.get<string>("VARMODVIEW_INPUT_VAR3", "");
		m_varModViewOutputVar3 = props.get<string>("VARMODVIEW_OUTPUT_VAR3", "");
		m_varModViewScript3 = props.get<string>("VARMODVIEW_SCRIPT3", "");

		m_varModViewInputVar4 = props.get<string>("VARMODVIEW_INPUT_VAR4", "");
		m_varModViewOutputVar4 = props.get<string>("VARMODVIEW_OUTPUT_VAR4", "");
		m_varModViewScript4 = props.get<string>("VARMODVIEW_SCRIPT4", "");

	}
	catch (std::exception&e)
	{
		::MessageBoxA(NULL, common::format("Error!!\n %s", e.what()).c_str(), "Error", MB_OK);
	}

	return true;
}


// 환경파일을 저장한다.
bool cMotionMonitorConfig::WriteConfigFile(const string &fileName)
{
	if (fileName.empty())
		return false;

	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		
		m_fileName = fileName;

		props.add<string>("MODE", m_mode);
		props.add<int>("UDP_PORT", m_udpPort);
		props.add<string>("UDP_PLOT_COMMAND", m_udpPlotCommand);
		props.add<string>("UDP_PLOTINPUT_COMMAND", m_udpPlotInputCmd);
		props.add<string>("UDP_MOD_COMMAND", m_udpModCommand);
		props.add<string>("MWAVE_MOD_COMMAND", m_mwaveModCommand);
		props.add<string>("JOYSTICK_MOD_COMMAND", m_joystickModCommand);
		props.add<string>("MIXING_COMMAND", m_mixingModCommand);
		props.add<int>("MOTION_OUTPUT_COMPORT", m_motionOutputComPort);
		props.add<int>("MOTION_OUTPUT_BAUDRATE", m_motionOutputBaudRate);

		props.add<int>("UDP_PARSE_PORT", m_udpParsePort);
		props.add<string>("UDP_PROTOCOL_COMMAND", m_udpProtocolCommand);
		props.add<string>("MIXING_SCRIPT", m_mixingScript);
		props.add<string>("UDP_INPUT_ROLL_COMMAND", m_rollCommand);
		props.add<string>("UDP_INPUT_PITCH_COMMAND", m_pitchCommand);
		props.add<string>("UDP_INPUT_YAW_COMMAND", m_yawCommand);
		props.add<string>("UDP_INPUT_HEAVE_COMMAND", m_heaveCommand);

		props.add<string>("PLOTVIEW_PLOT_COMMAND", m_plotViewPlotCmd);
		props.add<string>("PLOTVIEW_PLOTINPUT_COMMAND", m_plotViewPlotInputCmd);

		props.add<float>("DIRT3VIEW_PLAYTIME", m_dirt3ViewPlayTime);
		props.add<int>("DIRT3VIEW_AXISTYPE", m_dirt3ViewAxisType);
		props.add<float>("DIRT3VIEW_ACTUATOR_RANGE", m_dirt3ViewActuatorPower);
		props.add<float>("DIRT3VIEW_ACTUATOR_YAW_POWER", m_dirt3ViewActuatorYawPower);
		props.add<float>("DIRT3VIEW_ACTUATOR_SPEED", m_dirt3ViewActuatorSpeed);
		props.add<bool>("DIRT3VIEW_AUTOSTART", m_dirt3ViewAutoStart);

		props.add<string>("VARMODVIEW_INPUT_VAR", m_varModViewInputVar);
		props.add<string>("VARMODVIEW_OUTPUT_VAR", m_varModViewOutputVar);
		props.add<string>("VARMODVIEW_SCRIPT", m_varModViewScript);

		props.add<string>("VARMODVIEW_INPUT_VAR2", m_varModViewInputVar2);
		props.add<string>("VARMODVIEW_OUTPUT_VAR2", m_varModViewOutputVar2);
		props.add<string>("VARMODVIEW_SCRIPT2", m_varModViewScript2);

		props.add<string>("VARMODVIEW_INPUT_VAR3", m_varModViewInputVar3);
		props.add<string>("VARMODVIEW_OUTPUT_VAR3", m_varModViewOutputVar3);
		props.add<string>("VARMODVIEW_SCRIPT3", m_varModViewScript3);

		props.add<string>("VARMODVIEW_INPUT_VAR4", m_varModViewInputVar4);
		props.add<string>("VARMODVIEW_OUTPUT_VAR4", m_varModViewOutputVar4);
		props.add<string>("VARMODVIEW_SCRIPT4", m_varModViewScript4);

		boost::property_tree::write_json(fileName, props);
	}
	catch (std::exception&e)
	{
		//::AfxMessageBox(CString(L"Error!!\n") + str2wstr(e.what()).c_str());
		::MessageBoxA(NULL, common::format("Error!!\n %s", e.what()).c_str(), "Error", MB_OK);
	}

	return true;
}

