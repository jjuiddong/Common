
#include "stdafx.h"
#include "config.h"
#include <iostream>
#include <fstream>
#include <stdexcept>


using namespace cv;


cCamConfig g_config;


cCamConfig::cCamConfig()
{
}

cCamConfig::~cCamConfig()
{
}


// 환경설정 파일을 읽어서 저장한다.
bool cCamConfig::Parse(const string &fileName)
{
	std::ifstream cfgfile(fileName);
	if (!cfgfile.is_open())
	{
		printf("not exist config file!! <%s>\n", fileName.c_str());
		return false;
	}

	stringstream ss;
	ss << cfgfile.rdbuf();
	ParseStr(ss.str());

	return true;
}


bool cCamConfig::ParseStr(const string &str)
{
	using namespace std;

	stringstream ss(str);
	m_options.clear();

	string id, eq, val;
	while (ss >> id)// >> eq >> val)
	{
		if (id[0] == '#')
		{
			char line[128];
			ss.getline(line, sizeof(line));
			continue;  // skip comments
		}

		ss >> eq >> val;
		if (eq != "=") throw std::runtime_error("Parse error");
		m_options[id] = val;
	}

	UpdateParseData();

	printf("read config file\n");

	return true;
}


bool cCamConfig::GetBool(const string &key)
{
	return (atoi(m_options[key].c_str()) > 0) ? true : false;
}


float cCamConfig::GetFloat(const string &key)
{
	return (float)atof(m_options[key].c_str());
}


int cCamConfig::GetInt(const string &key)
{
	return atoi(m_options[key].c_str());
}


// config 파일을 읽은 후, 읽은 정보를 실제 변수에 할당한다.
void cCamConfig::UpdateParseData()
{
	m_conf.showCameraFrame = (atoi(m_options["camer_show_frame"].c_str()) > 0) ? true : false;
	m_conf.showCameraThreadFrame = (atoi(m_options["camera_show_thread_frame"].c_str()) > 0) ? true : false;
	m_conf.cameraFrameDelay = atoi(m_options["camera_frame_delay"].c_str());
	m_conf.cameraPlayer1Index = atoi(m_options["camera_player1_index"].c_str());
	m_conf.cameraPlayer2Index = atoi(m_options["camera_player2_index"].c_str());
	
	m_conf.showCamera = (atoi(m_options["camera_show"].c_str()) > 0) ? true : false;
	m_conf.showScreen = (atoi(m_options["screen_show"].c_str()) > 0) ? true : false;
	m_conf.showDetectPoint = (atoi(m_options["detectpoint_show"].c_str()) > 0) ? true : false;
	m_conf.showDebug = (atoi(m_options["debug_show"].c_str()) > 0) ? true : false;

// 	m_conf.stream_player1_enable = GetBool("streaming_player1_enable");
// 	m_conf.stream_player2_enable = GetBool("streaming_player2_enable");

	m_conf.stream_player = GetInt("streaming_player");
	m_conf.stream_server_port = GetInt("streaming_server_port");
	m_conf.stream_send_gray = GetBool("streaming_send_gray");
	m_conf.stream_send_compressed = GetBool("streaming_send_compressed");
	m_conf.streaming_send_quality = GetInt("streaming_send_quality");

	m_conf.printMachineGunPlayer1XY = GetBool("print_machinegun_player1_xy");
	m_conf.printMachineGunPlayer2XY = GetBool("print_machinegun_player2_xy");
	m_conf.pidControlP = GetFloat("pidcontrol_p");
	m_conf.pidControlI = GetFloat("pidcontrol_i");
	m_conf.pidControlD = GetFloat("pidcontrol_d");

	m_conf.monitor_mode = GetBool("monitor_mode");
	string monitor_ip = m_options["monitor_ip"];
	strcpy_s(m_conf.monitor_ip, monitor_ip.c_str());
	m_conf.monitor_port = GetInt("monitor_port");

	string client_ip = m_options["udp_client_connect_ip"];
	strcpy(m_conf.udpClientConnectIp, client_ip.c_str());
	m_conf.udpClientConnectPort = GetInt("udp_client_connect_port");

	m_gameMode = atoi(m_options["game_mode"].c_str());
	m_projectName = m_options["project_name"];
	m_printNetworkMessage = (atoi(m_options["print_network_message"].c_str()) > 0) ? true : false;
	m_cameraWidth = atoi(m_options["camera_width"].c_str());
	m_cameraHeight = atoi(m_options["camera_height"].c_str());


	m_cameraIndex = atoi(m_options["camera_index"].c_str());
	m_screen_BinaryThreshold = atoi(m_options["screen_binary_threshold"].c_str());
	m_rotateSensitivity = atoi(m_options["rotate_sensitivity"].c_str());


	m_cx = (float)atof(m_options["cx"].c_str());
	m_cy = (float)atof(m_options["cy"].c_str());
	m_fx = (float)atof(m_options["fx"].c_str());
	m_fy = (float)atof(m_options["fy"].c_str());
	m_k1 = (float)atof(m_options["k1"].c_str());
	m_k2 = (float)atof(m_options["k2"].c_str());
	m_k3 = (float)atof(m_options["k3"].c_str());
	m_p1 = (float)atof(m_options["p1"].c_str());
	m_p2 = (float)atof(m_options["p2"].c_str());
	m_skew_c = (float)atof(m_options["skew_c"].c_str());

	m_udpDevice1Port = atoi(m_options["udp_device1_port"].c_str());
	m_udpDevice2Port = atoi(m_options["udp_device2_port"].c_str());
	m_udpDevice3Port = atoi(m_options["udp_device3_port"].c_str());
	m_udpSleepMilliseconds = atoi(m_options["udp_sleep_millseconds"].c_str());

	m_printUDPDevice1Message = (atoi(m_options["print_udp_device1"].c_str()) > 0) ? true : false;
	m_printUDPDevice2Message = (atoi(m_options["print_udp_device2"].c_str()) > 0) ? true : false;
	m_printUDPDevice3Message = (atoi(m_options["print_udp_device3"].c_str()) > 0) ? true : false;

	m_udpClientConnectIp = m_options["udp_client_connect_ip"].c_str();
	m_udpClientConnectPort = atoi(m_options["udp_client_connect_port"].c_str());

	m_sharedMemName = m_options["sharedmemory_name"];


	{
		const string windowName = "screen";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[ windowName+"_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_screenRect = Rect(x, y, w, h);
		m_screenEnable = enable;
	}

	{
		const string windowName = "screen_binariztion";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_screenBinRect = Rect(x, y, w, h);
		m_screenBinEnable = enable;
	}

	{
		const string windowName = "screen_recognition";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_screenRecogRect = Rect(x, y, w, h);
		m_screenRecogEnable = enable;
	}

	{
		const string windowName = "camera";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_cameraRect = Rect(x, y, w, h);
		m_cameraEnable = enable;
	}

	{
		const string windowName = "skew_screen";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_skewScreenRect = Rect(x, y, w, h);
		m_skewScreenEnable = enable;
	}

	{
		const string windowName = "match_screen";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_matchScreenRect = Rect(x, y, w, h);
		m_matchScreenEnable = enable;

		m_matchScreenDrawBox = (atoi(m_options[windowName + "_drawbox"].c_str()) > 0) ? true : false;
	}

	{
		const string windowName = "detect_point_binary_screen";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_detectPointBinaryScreenRect = Rect(x, y, w, h);
		m_detectPointBinaryScreenEnable = enable;
	}
}


// 기본 설정 값으로 설정한다.
void cCamConfig::InitDefault()
{
	m_gameMode = 0;

	m_cameraWidth = 640;
	m_cameraHeight = 480;
	m_cameraIndex = 0;
	m_conf.cameraFrameDelay = 25;
	m_conf.cameraPlayer1Index = 0;
	m_conf.cameraPlayer2Index = 1;

	m_conf.showCamera = true;
	m_conf.showScreen = true;
	m_conf.showDetectPoint = true;
	m_conf.showDebug = true;
	m_conf.showCameraFrame = true;
	m_conf.showCameraThreadFrame = true;

	m_conf.stream_server_port = 8889;

	m_conf.printMachineGunPlayer1XY = false;
	m_conf.printMachineGunPlayer2XY = false;

	m_conf.pidControlP = 1;
	m_conf.pidControlI = 0;
	m_conf.pidControlD = 0;

	


	m_udpClientConnectIp = "127.0.0.1";
	m_udpClientConnectPort = 10000;

	m_screen_BinaryThreshold = 55;
	m_rotateSensitivity = 200;

	m_cx = 346.982762f;
	m_cy = 229.626156f;
	m_fx = 676.748756f;
	m_fy = 677.032333f;
	m_k1 = -0.331818f;
	m_k2 = -0.033314f;
	m_k3 = 0.f;
	m_p1 = -0.000313f;
	m_p2 = 0.000841f;
	m_skew_c = 0;

	// windows
	m_screenRect = Rect(0, 0, 1024, 768);
	m_cameraRect = Rect(-660, 0, 640, 480);
	m_screenBinRect = Rect(-660, 520, 640, 480);
	m_screenRecogRect = Rect(-1320, 520, 640, 480);
	m_skewScreenRect = Rect(-660, 520, 640, 480);
	m_matchScreenRect = Rect(-1680, 0, 640, 480);
	m_detectPointBinaryScreenRect = Rect(-1310, 520, 640, 480);

	m_screenEnable = true;
	m_cameraEnable = true;
	m_screenBinEnable = true;
	m_screenRecogEnable = true;
	m_skewScreenEnable = true;
	m_matchScreenEnable = true;
	m_detectPointBinaryScreenEnable = true;

	m_matchScreenDrawBox = false;

	m_udpDevice1Port = 1234;
	m_udpDevice2Port = 1234;
	m_udpDevice3Port = 1234;
	m_udpSleepMilliseconds = 20;

	m_printUDPDevice1Message = true;
	m_printUDPDevice2Message = true;
	m_printUDPDevice3Message = true;

	m_changeStreamingOption = false;
}


bool cCamConfig::Write(const string &fileName)
{
	using namespace std;

	m_options["camer_show_frame"] = common::format("%d", m_conf.showCameraFrame);
	m_options["camera_show_thread_frame"] = common::format("%d", m_conf.showCameraThreadFrame);
	m_options["camera_frame_delay"] = common::format("%d", m_conf.cameraFrameDelay);
	m_options["camera_player1_index"] = common::format("%d", m_conf.cameraPlayer1Index);
	m_options["camera_player2_index"] = common::format("%d", m_conf.cameraPlayer2Index);
	
	m_options["camera_show"] = common::format("%d", m_conf.showCamera);
	m_options["screen_show"] = common::format("%d", m_conf.showScreen);
	m_options["detectpoint_show"] = common::format("%d", m_conf.showDetectPoint);
	m_options["debug_show"] = common::format("%d", m_conf.showDebug);

	m_options["streaming_player"] = common::format("%d", m_conf.stream_player);
	m_options["streaming_server_port"] = common::format("%d", m_conf.stream_server_port);
	m_options["streaming_send_gray"] = common::format("%d", m_conf.stream_send_gray);
	m_options["streaming_send_compressed"] = common::format("%d", m_conf.stream_send_compressed);
	m_options["streaming_send_quality"] = common::format("%d", m_conf.streaming_send_quality);


	m_options["print_machinegun_player1_xy"] = common::format("%d", m_conf.printMachineGunPlayer1XY);
	m_options["print_machinegun_player2_xy"] = common::format("%d", m_conf.printMachineGunPlayer2XY);
	m_options["pidcontrol_p"] = common::format("%f", m_conf.pidControlP);
	m_options["pidcontrol_i"] = common::format("%f", m_conf.pidControlI);
	m_options["pidcontrol_d"] = common::format("%f", m_conf.pidControlD);

	m_options["monitor_mode"] = common::format("%d", m_conf.monitor_mode);
	m_options["monitor_ip"] = m_conf.monitor_ip;
	m_options["monitor_port"] = common::format("%d", m_conf.monitor_port);

	m_options["udp_client_connect_ip"] = m_conf.udpClientConnectIp;
	m_options["udp_client_connect_port"] = common::format("%d", m_conf.udpClientConnectPort);


	// 동일한 이름에 파일이 있다면, 그 파일 형태를 그대로 유지하면서,
	// 데이타만 업데이트 되는 형태로 저장하게 한다.
	std::ifstream cfgfile(fileName);
	if (!cfgfile.is_open())
	{
		// 동일한 파일이 없다면, 그대로 저장하고 종료한다.
		ofstream ofs(fileName);
		if (!ofs.is_open())
			return false;

		for each (auto &kv in m_options)
		{
			if (kv.second.empty())
				ofs << kv.first << " = " << "0" << endl;
			else
				ofs << kv.first << " = " << kv.second << endl;
		}
		return false;
	}

	set<string> checkId; //저장 된 정보와 그렇지 않은 정보를 찾기위해 필요함.

	stringstream ss;
	ss << cfgfile.rdbuf();

	stringstream savess;
	char line[128];
	while (ss.getline(line, sizeof(line)))
	{
		stringstream ssline(line);

		string id, eq, val;
		ssline >> id;

		if (id[0] == '#') // 주석
		{
			savess << line << endl;
			continue;
		}		
		if (id.empty()) // 개행 문자
		{
			savess << line << endl;
			continue;
		}

		ssline >> eq >> val;
		if (eq != "=") throw std::runtime_error("Parse error");

		if (m_options[id].empty())
			savess << id << " = " << 0 << endl;
		else
			savess << id << " = " << m_options[id] << endl;

		checkId.insert(id);
	}

	// Write되지 않은 Id가 있다면, 검색해서, 저장한다.
	for each (auto &kv in m_options)
	{
		const auto it = checkId.find(kv.first);
		if (checkId.end() == it)
		{
			if (kv.second.empty())
				savess << kv.first << " = " << 0 << endl;
			else
				savess << kv.first << " = " << kv.second << endl;
		}
	}

	ofstream ofs(fileName);
	if (!ofs.is_open())
		return false;
	ofs << savess.rdbuf();

	return true;
}
