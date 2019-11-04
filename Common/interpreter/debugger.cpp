
#include "stdafx.h"
#include "debugger.h"


using namespace common;
using namespace common::script;


cDebugger::cDebugger()
	: m_interpreter(nullptr)
	, m_state(eState::Stop)
{
}

cDebugger::~cDebugger()
{
	Clear();
}


bool cDebugger::Init(cInterpreter *interpreter)
{
	Clear();

	m_interpreter = interpreter;
	m_interpreter->DebugRun();
	m_state = eState::DebugWait;
	return true;
}


bool cDebugger::Process(const float deltaSeconds)
{
	//m_interpreter->ProcessOneStep(0.1f);
	switch (m_state)
	{
	case eState::Stop:
	case eState::DebugWait:
		break;
	case eState::DebugStep:
		m_interpreter->ProcessOneStep(0.1f);
		m_state = eState::DebugWait;
		break;
	case eState::DebugRun:
		m_interpreter->ProcessOneStep(0.1f);
		break;
	default:
		assert(!"cDebugger::Process() not vailid state");
		break;
	}
	return true;
}


bool cDebugger::OneStep()
{
	RETV(!m_interpreter, false);

	if (m_interpreter->IsStop())
		m_interpreter->DebugRun(); // start debugging

	m_state = eState::DebugStep;
	return true;
}


// cancel debugging, run interpreter
bool cDebugger::Run()
{
	RETV(!m_interpreter, false);
	
	if (m_interpreter->IsDebug())
	{
		m_interpreter->DebugToRun(); // run state
		m_state = eState::DebugRun;
	}
	return true;
}


// break interpreter running state, and then change debug state
bool cDebugger::Break()
{
	RETV(!m_interpreter, false);

	if (m_interpreter->IsRun())
	{
		m_interpreter->DebugBreak(); // debug state
		m_state = eState::DebugWait;
	}
	return true;
}


// terminate debugging state
bool cDebugger::Terminate()
{
	RETV(!m_interpreter, false);

	m_interpreter->DebugCancel(); // stop state
	m_state = eState::Stop;
	return true;
}


void cDebugger::Clear()
{
	m_interpreter = nullptr;
	m_state = eState::Stop;
}
