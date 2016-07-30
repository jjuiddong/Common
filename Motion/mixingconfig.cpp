
#include "stdafx.h"
#include "mixingconfig.h"


const string g_minxingConfigFileName = "mixing_conf.cfg";

cMixingConfig::cMixingConfig():
	m_bias_yaw(0)
, m_bias_pitch(0)
, m_bias_roll(0)
, m_bias_heave(0)
,m_rate1_center_yaw(0)
,m_rate1_center_pitch(0)
,m_rate1_center_roll(0)
,m_rate1_center_heave(0)
,m_rate2_center_yaw(0)
,m_rate2_center_pitch(0)
,m_rate2_center_roll(0)
,m_rate2_center_heave(0)
,m_rate3_center_yaw(0)
,m_rate3_center_pitch(0)
,m_rate3_center_roll(0)
,m_rate3_center_heave(0)
{

}

cMixingConfig::~cMixingConfig()
{
	//Save(g_minxingConfigFileName);
}


void cMixingConfig::InitDefault() 
{
	m_inputType = 0;

	m_input1_enable = true;
	m_rate1_all = 1;
	m_rate1_yaw = 1;
	m_rate1_pitch = 1;
	m_rate1_roll = 1;
	m_rate1_heave = 1;

	m_input2_enable = false;
	m_rate2_all = 1;
	m_rate2_yaw = 1;
	m_rate2_pitch = 1;
	m_rate2_roll = 1;
	m_rate2_heave = 1;

	m_input3_enable = false;
	m_rate3_all = 1;
	m_rate3_yaw = 1;
	m_rate3_pitch = 1;
	m_rate3_roll = 1;
	m_rate3_heave = 1;

	m_input4_enable = false;
	m_rate4_all  = 1;
	m_rate4_pitch = 1;
	m_rate4_roll = 1;
	m_rate4_center_pitch = 0;
	m_rate4_center_roll = 0;

	m_bias_yaw = 0;
	m_bias_pitch = 0;
	m_bias_roll = 0;
	m_bias_heave = 0;

	m_rate1_center_yaw= 0;
	m_rate1_center_pitch= 0;
	m_rate1_center_roll= 0;
	m_rate1_center_heave= 0;
	m_rate2_center_yaw= 0;
	m_rate2_center_pitch= 0;
	m_rate2_center_roll= 0;
	m_rate2_center_heave= 0;
	m_rate3_center_yaw= 0;
	m_rate3_center_pitch= 0;
	m_rate3_center_roll= 0;
	m_rate3_center_heave= 0;

}


void cMixingConfig::UpdateParseData()
{
	m_inputType = 0;

	const string input = m_options["input"];
	if (!input.empty())
	{
		vector<string> toks;
		common::tokenizer(input, "+", "", toks);

		for each (auto &str in toks)
		{
			if (str == "joystick")
			{
				m_inputType |= INPUT_JOYSTICK;
			}
			else if (str == "udp")
			{
				m_inputType |= INPUT_UDP;
			}
			else if (str == "motionwave")
			{
				m_inputType |= INPUT_MOTIONWAVE;
			}
			else if (str == "var")
			{
				m_inputType |= INPUT_VAR;
			}
		}
	}

	m_input1_enable = GetBool("mixing_input1_enable");
	m_rate1_all = GetFloat("mixing_rate1_all");
	m_rate1_yaw = GetFloat("mixing_rate1_yaw");
	m_rate1_pitch = GetFloat("mixing_rate1_pitch");
	m_rate1_roll = GetFloat("mixing_rate1_roll");
	m_rate1_heave = GetFloat("mixing_rate1_heave");

	m_input2_enable = GetBool("mixing_input2_enable");
	m_rate2_all = GetFloat("mixing_rate2_all");
	m_rate2_yaw = GetFloat("mixing_rate2_yaw");
	m_rate2_pitch = GetFloat("mixing_rate2_pitch");
	m_rate2_roll = GetFloat("mixing_rate2_roll");
	m_rate2_heave = GetFloat("mixing_rate2_heave");

	m_input3_enable = GetBool("mixing_input3_enable");
	m_rate3_all = GetFloat("mixing_rate3_all");
	m_rate3_yaw = GetFloat("mixing_rate3_yaw");
	m_rate3_pitch = GetFloat("mixing_rate3_pitch");
	m_rate3_roll = GetFloat("mixing_rate3_roll");
	m_rate3_heave = GetFloat("mixing_rate3_heave");

	m_input4_enable = GetBool("mixing_input4_enable");
	m_rate4_all = GetFloat("mixing_rate4_all");
	m_rate4_pitch = GetFloat("mixing_rate4_pitch");
	m_rate4_roll = GetFloat("mixing_rate4_roll");
	m_rate4_center_pitch = GetFloat("mixing_rate4_center_pitch", 0);
	m_rate4_center_roll = GetFloat("mixing_rate4_center_roll", 0);

	m_bias_yaw = GetFloat("mixing_bias_yaw", 0);
	m_bias_pitch = GetFloat("mixing_bias_pitch", 0);
	m_bias_roll = GetFloat("mixing_bias_roll", 0);
	m_bias_heave = GetFloat("mixing_bias_heave", 0);

	m_rate1_center_yaw = GetFloat("mixing_rate1_center_yaw", 0);
	m_rate1_center_pitch = GetFloat("mixing_rate1_center_pitch", 0);
	m_rate1_center_roll = GetFloat("mixing_rate1_center_roll", 0);
	m_rate1_center_heave = GetFloat("mixing_rate1_center_heave", 0);

	m_rate2_center_yaw = GetFloat("mixing_rate2_center_yaw", 0);
	m_rate2_center_pitch = GetFloat("mixing_rate2_center_pitch", 0);
	m_rate2_center_roll = GetFloat("mixing_rate2_center_roll", 0);
	m_rate2_center_heave = GetFloat("mixing_rate2_center_heave", 0);

	m_rate3_center_yaw = GetFloat("mixing_rate3_center_yaw", 0);
	m_rate3_center_pitch = GetFloat("mixing_rate3_center_pitch", 0);
	m_rate3_center_roll = GetFloat("mixing_rate3_center_roll", 0);
	m_rate3_center_heave = GetFloat("mixing_rate3_center_heave", 0);
}
