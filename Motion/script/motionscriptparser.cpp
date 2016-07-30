
#include "stdafx.h"
#include "motionscriptparser.h"
#include "scriptutil.h"

using namespace motion;
using namespace script;


cMotionScriptParser::cMotionScriptParser() 
	: m_program(NULL)
{
}

cMotionScriptParser::~cMotionScriptParser()
{
	rm_program(m_program);
	m_program = NULL;
}


// 파일을 읽어서 파싱한다.
bool cMotionScriptParser::Read(const string &fileName)
{
	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
	{
		dbg::ErrLog("Not Found File [%s] \n", fileName.c_str());
		return false;
	}

	std::string strCmd((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	common::trim(strCmd);
	if (!Parse(strCmd))
		return false;
	return true;
}


// 문자열로 파싱한다.
bool cMotionScriptParser::Parse(const string &str)
{
	string source = str;
	rm_program(m_program);
	m_program = program(source);
	return true;
}


// program -> command_list component_list
sProgram* cMotionScriptParser::program(string &src)
{
	trim(src);

	if (src.empty())
		return NULL;

	sProgram *p = new sProgram;
	p->cmd = command_list(src);
	p->compList = component_list(src);

	return p;
}


// command_list -> { command }
// command -> id = string
sCommand* cMotionScriptParser::command_list(string &src)
{
	sCommand *p = new sCommand;
	while (1)
	{
		trim(src);
		if (src.empty())
			break;

		if (src[0] == '#') // line comment
		{
			line(src);
			continue;
		}

		string id1 = id(src);
		if (id1.empty())
			break;	

		trim(src);

		if (!check(src, '='))
			break;
		match(src, '=');

		p->values[id1] = trim(line(src));
	}

	return p;
}


// component_list -> { component }
sComponentList* cMotionScriptParser::component_list(string &src)
{
	sComponent *comp = component(src);
	if (!comp)
		return NULL;

	sComponentList *p = new sComponentList{ NULL, NULL };
	p->comp = comp;
	p->next = component_list(src);
	return p;
}


// component -> comp_header '{' comp_body '}'
sComponent* cMotionScriptParser::component(string &src)
{
	trim(src);

	sComponent *p = new sComponent;
	p->header = component_header(src);
	if (!p->header)
	{
		delete p;
		return NULL;
	}

	trim(src);
	match(src, '{');
	p->cmd = command_list(src);
	p->subCompList = subcomponent_list(src);
	trim(src);
	match(src, '}');

	if (!p->cmd && !p->subCompList)
	{
		delete p;
		return NULL;
	}

	// component type
	p->type = GetComponentType(p->header->id);
	if (((p->type == COMPONENT_TYPE::OUTPUT) || (p->type == COMPONENT_TYPE::INPUT))
		&& p->cmd)
	{
		p->devType = GetDeviceType(p->cmd->values["device"]);
	}
	
	return p;
}


// comp_header -> :id [id]
sComponentHeader* cMotionScriptParser::component_header(string &src)
{
	trim(src);

	if (!check(src, ':'))
		return NULL;

	match(src, ':');
	sComponentHeader *p = new sComponentHeader;
	p->id = id(src);
	p->filename = trim(line(src));
	return p;
}


// subcomponent_list -> { subcomponent }
sSubComponentList* cMotionScriptParser::subcomponent_list(string &src)
{
	trim(src);

	sSubComponent *comp = subcomponent(src);
	if (!comp)
		return NULL;

	sSubComponentList *p = new sSubComponentList;
	p->subComp = comp;
	p->next = subcomponent_list(src);

	return p;
}


// subcomponent ->  protocol | math | modulation | format
sSubComponent* cMotionScriptParser::subcomponent(string &src)
{
	trim(src);

	sComponentHeader *header = component_header(src);
	if (!header)
		return NULL;

	sSubComponent *p = new sSubComponent;
	p->header = header;
	
	trim(src);
	match(src, '{');
	
	while (1)
	{
		string s = line(src);
		if (s.empty())
			continue;;
		
		string tok = s;
		trim(tok);
		if (tok == "}")
			break;

		p->source += s;
		p->source += '\n';
	}

	// sub component type
	p->type = GetSubComponentType(p->header->id);

	return p;
}


SUBCOMPONENT_TYPE::TYPE cMotionScriptParser::GetSubComponentType(const string &str)
{
	if (str == "protocol")
	{
		return SUBCOMPONENT_TYPE::PROTOCOL;
	}
	else if (str == "init")
	{
		return SUBCOMPONENT_TYPE::INIT;
	}
	else if (str == "math")
	{
		return SUBCOMPONENT_TYPE::MATH;
	}
	else if (str == "modulation")
	{
		return SUBCOMPONENT_TYPE::MODULATION;
	}
	else if (str == "format")
	{
		return SUBCOMPONENT_TYPE::FORMAT;
	}
	else
	{
		dbg::ErrLog("not found sub component type %s \n", str.c_str());
		assert(0);
	}

	return SUBCOMPONENT_TYPE::NONE;
}


DEVICE_TYPE::TYPE cMotionScriptParser::GetDeviceType(const string &str)
{
	if (str == "shm")
	{
		return DEVICE_TYPE::SHAREDMEM;
	}
	else 	if (str == "udp")
	{
		return DEVICE_TYPE::UDP;
	}
	else 	if (str == "tcp")
	{
		return DEVICE_TYPE::TCP;
	}
	else 	if (str == "serial")
	{
		return DEVICE_TYPE::SERIAL;
	}
	else 	if (str == "joystick")
	{
		return DEVICE_TYPE::JOYSTICK;
	}
	else
	{
		dbg::ErrLog("not found device type %s \n", str.c_str());
		assert(0);
	}

	return DEVICE_TYPE::NONE;
}


COMPONENT_TYPE::TYPE cMotionScriptParser::GetComponentType(const string &str)
{
	if (str == "input")
	{
		return COMPONENT_TYPE::INPUT;
	}
	else 	if (str == "output")
	{
		return COMPONENT_TYPE::OUTPUT;
	}
	else 	if (str == "mixer")
	{
		return COMPONENT_TYPE::MIXER;
	}
	else 	if (str == "module")
	{
		return COMPONENT_TYPE::MODULE;
	}
	else 	if (str == "amplitude_module")
	{
		return COMPONENT_TYPE::AMPLITUDE_MODULE;
	}
	else
	{
		dbg::ErrLog("not found component type %s \n", str.c_str());
		assert(0);
	}

	return COMPONENT_TYPE::NONE;
}
