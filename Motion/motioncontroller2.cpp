
#include "stdafx.h"
#include "MotionController2.h"


using namespace motion;


cController2::cController2() :
	m_state(MODULE_STATE::STOP)
{
}

cController2::~cController2()
{
	Clear();
}


// 모션 스크립트 읽기
bool cController2::Init(const HWND hWnd, const string &fileName)
{
	dbg::Log("----------------------------------------------------------------- \n");
	dbg::Log("Read Motion Script %s \n", fileName.c_str());

	m_hWnd = hWnd;

	if (!m_script.Read(fileName))
	{
		dbg::ErrLog("Error Read Motion Script \n");
		return false;
	}

	Clear();
	return MotionScriptInterpreter();
}


bool cController2::Reload(const string &fileName)
{
	dbg::Log("----------------------------------------------------------------- \n");
	dbg::Log("Reload Motion Script %s \n", fileName.c_str());

	if (!m_script.Read(fileName))
	{
		dbg::ErrLog("Error Read Motion Script \n");
		return false;
	}

	const MODULE_STATE::TYPE oldState = m_state;

	Clear();

	if (MotionScriptInterpreter())
	{
		if (oldState == MODULE_STATE::START)
			Start();
	}

	return false;
}


bool cController2::AddInput(cInput *input)
{
	m_inputs.push_back(input);
	return true;
}


bool cController2::AddOutput(cOutput *output)
{
	m_outputs.push_back(output);
	return true;
}


bool cController2::AddMixer(cMixer *mixer)
{
	m_mixers.push_back(mixer);
	return true;
}

bool cController2::AddModule(cModule *module)
{
	m_modules.push_back(module);
	return true;
}

bool cController2::AddPostModule(cModule *module)
{
	m_postModules.push_back(module);
	return true;
}

cInput* cController2::GetInput(const int index)
{
	RETV((int)m_inputs.size() <= index, NULL);
	return m_inputs[index];
}


cOutput* cController2::GetOutput(const int index)
{
	RETV((int)m_outputs.size() <= index, NULL);
	return m_outputs[index];
}


cMixer* cController2::GetMixer(const int index)
{
	RETV((int)m_mixers.size() <= index, NULL);
	return m_mixers[index];
}


cModule* cController2::GetModule(const int index)
{
	RETV((int)m_modules.size() <= index, NULL);
	return m_modules[index];
}

cModule* cController2::GetPostModule(const int index)
{
	RETV((int)m_postModules.size() <= index, NULL);
	return m_postModules[index];
}

void cController2::Clear()
{
	Stop();

	for each(auto &obj in m_inputs)
		delete obj;
	for each(auto &obj in m_modules)
		delete obj;
	for each(auto &obj in m_postModules)
		delete obj;
	for each(auto &obj in m_mixers)
		delete obj;
	for each(auto &obj in m_outputs)
		delete obj;

	m_inputs.clear();
	m_modules.clear();
	m_postModules.clear();
	m_mixers.clear();
	m_outputs.clear();
}


bool cController2::Start()
{
	m_state = MODULE_STATE::START;

	dbg::Log("Start Motion Controller \n");

	for each(auto &obj in m_inputs)
		obj->Start();
	for each(auto &obj in m_modules)
		obj->Start();
	for each(auto &obj in m_mixers)
		obj->Start();
	for each(auto &obj in m_postModules)
		obj->Start();
	for each(auto &obj in m_outputs)
		obj->Start();

	return true;
}


bool cController2::Stop()
{
	m_state = MODULE_STATE::STOP;

	dbg::Log("Stop Motion Controller \n");

	for each(auto &obj in m_inputs)
		obj->Stop();
	for each(auto &obj in m_modules)
		obj->Stop();
	for each(auto &obj in m_mixers)
		obj->Stop();
	for each(auto &obj in m_postModules)
		obj->Stop();
	for each(auto &obj in m_outputs)
		obj->Stop();

	return true;
}


// Input -> Mixer -> Output
bool cController2::Update(const float deltaSeconds)
{
	RETV(MODULE_STATE::STOP == m_state, false);

	for each(auto &obj in m_inputs)
		obj->Update(deltaSeconds);
	for each(auto &obj in m_modules)
		obj->Update(deltaSeconds);
	for each(auto &obj in m_mixers)
		obj->Update(deltaSeconds);
	for each(auto &obj in m_postModules)
		obj->Update(deltaSeconds);
	for each(auto &obj in m_outputs)
		obj->Update(deltaSeconds);


	//--------------------------------------------------------------------
	// debug process
	// write shared memory 
	if (m_shmSymbols.m_map)
	{
		for each (auto sym in script::g_symbols)
			m_shmSymbols[sym.first] = sym.second;
	}
	//--------------------------------------------------------------------

	return true;
}


// outputIndex 번째 Output 객체의 포맷 인덱스를 설정한다.
void cController2::SetOutputFormat(const int outputIndex, const int formatIndex)
{
	if ((int)m_outputs.size() > outputIndex)
	{
		m_outputs[outputIndex]->SetFormat(formatIndex);
	}
}


// 모션 스크립트 실행
bool cController2::MotionScriptInterpreter()
{
	RETV(!m_script.m_program, false);

	if (m_script.m_program->cmd)
	{
		script::g_isLog = (atoi(m_script.m_program->cmd->values["log"].c_str()) > 0) ? true : false;
		const string shmName = m_script.m_program->cmd->values["sharedmemory_name"];
		if (!shmName.empty())
		{
			if (!m_shmSymbols.Open(shmName))
			{
				if (m_shmSymbols.Init(shmName))
				{
					dbg::Log("Init Shared Memory %s \n", shmName.c_str());
				}
				else
				{
					dbg::Log("- Error!! Init Shared Memory %s \n", shmName.c_str());
				}
			}
			else
			{
				dbg::Log("Open Shared Memory %s \n", shmName.c_str());
			}
		}
	}

	sComponentList *compList = m_script.m_program->compList;
	while (compList)
	{
		sComponent *comp = compList->comp;
		CreateComponent(comp);
		compList = compList->next;
	}

	return true;
}


bool cController2::CreateComponent(const sComponent *parentComp)
{
	RETV(!parentComp, false);

	cUDPInput *udpInput = NULL;
	cUDPOutput *udpOutput = NULL;
	cSerialOutput *serialOutput = NULL;
	cShmemInput *shmInput = NULL;
	cJoyStickInput *joystickInput = NULL;
	cNormalMixer *mixer = NULL;
	cSimpleModule *module = NULL;
	cAmplitudeModule *ampModule = NULL;
	string *strProtocol = NULL;
	string *strInit = NULL;
	string *strMath = NULL;
	string *strModulation = NULL;
	vector<string*> strFormats;

	// Create Input/Output/Mixer/Module
	switch (parentComp->type)
	{
	case COMPONENT_TYPE::INPUT:
	{
		switch (parentComp->devType)
		{
		case DEVICE_TYPE::UDP:  udpInput = new cUDPInput(); break;
		case DEVICE_TYPE::SERIAL: break;
		case DEVICE_TYPE::SHAREDMEM: shmInput = new cShmemInput(); break;
		case DEVICE_TYPE::JOYSTICK: joystickInput = new cJoyStickInput(); break;
		default:
			dbg::ErrLog("not found device type [%d] \n", parentComp->devType);
			assert(0);
			break;
		}
	}
	break;

	case COMPONENT_TYPE::OUTPUT:
	{
		switch (parentComp->devType)
		{
		case DEVICE_TYPE::UDP:  udpOutput = new cUDPOutput(); break;
		case DEVICE_TYPE::SERIAL: serialOutput = new cSerialOutput(); break;
		default: 
			dbg::ErrLog("not found component device type [%d] \n", parentComp->devType);
			assert(0); 
			break;
		}
	}
	break;

	case COMPONENT_TYPE::MIXER: 
		mixer = new cNormalMixer();
		break;

	case COMPONENT_TYPE::MODULE:
		module = new cSimpleModule();
		break;

	case COMPONENT_TYPE::AMPLITUDE_MODULE:
		ampModule = new cAmplitudeModule();
		break;

	default:
		dbg::ErrLog("not found component type \n");
		assert(0);
		break;
	}
	//-----------------------------------------------------------------------------


	sSubComponentList *currentSubCompList = parentComp->subCompList;
	while (currentSubCompList)
	{
		sSubComponent *subComp = currentSubCompList->subComp;
		if (!subComp)
			break;

		switch (subComp->type)
		{
		case SUBCOMPONENT_TYPE::PROTOCOL: strProtocol = &subComp->source; break;
		case SUBCOMPONENT_TYPE::INIT: strInit = &subComp->source; break;
		case SUBCOMPONENT_TYPE::MATH: strMath = &subComp->source; break;
		case SUBCOMPONENT_TYPE::MODULATION: strModulation = &subComp->source; break;
		case SUBCOMPONENT_TYPE::FORMAT: strFormats.push_back( &subComp->source ); break;
		default: 
			dbg::ErrLog("not found sub component type [%d] \n", subComp->type);
			assert(0); 
			break;
		}
	
		currentSubCompList = currentSubCompList->next;
	}


	//-------------------------------------------------------------------
	// Create Component

	// Create UDP Input
	if (udpInput)
	{
		if (!strProtocol)
		{
			dbg::ErrLog("not found protocol code\n");
		}
		if (!strMath)
		{
			dbg::ErrLog("not found math code\n");
		}
		if (!strModulation)
		{
			dbg::ErrLog("not found modulation code\n");
		}

		int bindPort = 0;
		if (parentComp->cmd)
			bindPort = atoi(parentComp->cmd->values["bindport"].c_str());

		int startIndex = 1;
		if (parentComp->cmd)
		{
			const string strStartIndex = parentComp->cmd->values["startindex"];
			startIndex = (strStartIndex.empty()) ? 1 : atoi(strStartIndex.c_str());
		}

		if (udpInput->Init(startIndex, bindPort, strProtocol ? *strProtocol : "",
			strMath ? *strMath : "", strModulation ? *strModulation : ""))
		{
			AddInput(udpInput);
			dbg::Log("Success create udp input bindPort = %d \n", bindPort);
		}
		else
		{
			dbg::ErrLog("error create udpinput\n");
		}
	}
	
	// Create Shared Memory Input
	if (shmInput)
	{
		if (!strProtocol)
		{
			dbg::ErrLog("not found protocol code\n");
		}
		if (!strMath)
		{
			dbg::ErrLog("not found math code\n");
		}
		if (!strModulation)
		{
			dbg::ErrLog("not found modulation code\n");
		}

		string shmName;
		if (parentComp->cmd)
			shmName = trim(parentComp->cmd->values["shmname"]);

		int startIndex = 1001;
		int memorySize = 1024;
		if (parentComp->cmd)
		{
			const string strStartIndex = parentComp->cmd->values["startindex"];
			startIndex = (strStartIndex.empty())? 1001 : atoi(strStartIndex.c_str());

			const string strMemorySize = parentComp->cmd->values["memsize"];
			memorySize = (strMemorySize.empty()) ? 1001 : atoi(strMemorySize.c_str());
		}

		if (shmInput->Init(startIndex, memorySize, shmName, strProtocol ? *strProtocol : "",
			strMath ? *strMath : "", strModulation ? *strModulation : ""))
		{
			AddInput(shmInput);
			dbg::Log("Success create shared memory input shmname = %s \n", shmName.c_str());
		}
		else
		{
			dbg::ErrLog("error create shared memory input \n");
		}
	}

	// Create Joytstick Input
	if (joystickInput)
	{
		if (!strMath)
		{
			dbg::ErrLog("not found math code\n");
		}
		if (!strModulation)
		{
			dbg::ErrLog("not found modulation code\n");
		}

		if (joystickInput->Init(cJoyStickInput::JOYPAD, m_hWnd, 
			strMath ? *strMath : "", strModulation ? *strModulation : ""))
		{
			AddInput(joystickInput);
			dbg::Log("Success create Joystick input \n");
		}
		else
		{
			dbg::ErrLog("error create Joystick input \n");
		}
	}

	// Create Serial Output
	if (serialOutput)
	{
		if (strFormats.empty())
		{
			dbg::ErrLog("not found format code \n");
		}

		int portNum = 0;
		int baudRate = 0;
		string outputLog;
		if (parentComp->cmd)
		{
			portNum = atoi(parentComp->cmd->values["portnum"].c_str());
			baudRate = atoi(parentComp->cmd->values["baudrate"].c_str());
			outputLog = parentComp->cmd->values["outputlog"];
		}

		if (serialOutput->Init(portNum, baudRate, outputLog, strFormats))
		{
			AddOutput(serialOutput);
			dbg::Log("Success create serial output portNum = %d, baudRate = %d, outputLog = %s \n", portNum, baudRate, outputLog.c_str());
		}
		else
		{
			dbg::ErrLog("error create serial output \n");
		}
	}

	// Create UDP Output
	if (udpOutput)
	{
		if (strFormats.empty())
		{
			dbg::ErrLog("not found format code \n");
		}

		string ip = "127.0.0.1";
 		int port = 8888;
		string outputLog;
		if (parentComp->cmd)
		{
			port = atoi(parentComp->cmd->values["port"].c_str());
 			ip = parentComp->cmd->values["ip"];
			outputLog = parentComp->cmd->values["outputlog"];
		}

		if (udpOutput->Init(ip, port, outputLog, strFormats))
		{
			AddOutput(udpOutput);
			dbg::Log("Success create udp output ip=%s, port=%d \n", ip.c_str(), port);
		}
		else
		{
			dbg::ErrLog("error create udp output \n");
		}
	}

	// Create Mixer
	if (mixer)
	{
		if (mixer->Init(strInit? *strInit : "", strMath ? *strMath : ""))
		{
			AddMixer(mixer);
			dbg::Log("Success create mixer \n");
		}
		else
		{
			dbg::ErrLog("error create mixer \n");
		}
	}

	// Create Module
	if (module)
	{
		const string inputValue = parentComp->cmd->values["inputvalue"];
		const string outputValue = parentComp->cmd->values["outputvalue"];

		if (module->Init(inputValue, outputValue, strMath ? *strMath : "", strModulation? *strModulation : ""))
		{
			AddModule(module);
			dbg::Log("Success create module input = %s, output = %s \n", inputValue.c_str(), outputValue.c_str());
		}
		else
		{
			dbg::ErrLog("error create module \n");
		}
	}

	// Create Amplitude Module
	if (ampModule)
	{
		if (ampModule->Init(strMath ? *strMath : "",  strModulation ? *strModulation : ""))
		{
			AddPostModule(ampModule);
			dbg::Log("Success create amplitude_module  \n");
		}
		else
		{
			dbg::ErrLog("error create amplitude_module \n");
		}
	}

	return true;
}
