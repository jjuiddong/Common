
#include "stdafx.h"
#include "debugger.h"


using namespace common;
using namespace common::script;


cDebugger::cDebugger(cInterpreter *interpreter //= nullptr
)
	: m_interpreter(nullptr)
	, m_state(eState::Stop)
{
	if (interpreter)
		Init(interpreter);
}

cDebugger::~cDebugger()
{
	Clear();
}


// initialize debugger
bool cDebugger::Init(cInterpreter *interpreter)
{
	Clear();

	m_interpreter = interpreter;
	return true;
}


// load intermediate code, and then DebugRun state
bool cDebugger::LoadIntermediateCode(const StrPath &fileName)
{
	RETV(!m_interpreter, false);

	if (!m_interpreter->IsStop())
		return false; // error, already running~

	if (!m_interpreter->ReadIntermediateCode(fileName))
		return false;

	m_interpreter->DebugRun();
	m_state = eState::DebugWait;
	return true;
}


// process debugger
bool cDebugger::Process(const float deltaSeconds)
{
	RETV(!m_interpreter, false);

	switch (m_state)
	{
	case eState::Stop:
	case eState::DebugWait:
		break;
	case eState::DebugStep:
		m_interpreter->ProcessOneStep(deltaSeconds);
		m_state = eState::DebugWait;
		break;
	case eState::DebugRun:
		m_interpreter->Process(deltaSeconds);
		break;
	default:
		assert(!"cDebugger::Process() not vailid state");
		break;
	}
	return true;
}


// one step debugging
bool cDebugger::OneStep()
{
	RETV(!m_interpreter, false);

	if (m_interpreter->IsStop())
		m_interpreter->DebugRun(); // start debugging
	else
		m_interpreter->DebugBreak(); // debug mode

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

	m_interpreter->Stop(); // stop state
	m_state = eState::Stop;
	return true;
}


// is load interpreter
bool cDebugger::IsLoad()
{
	return m_interpreter ? true : false;
}


// is run interpreter
bool cDebugger::IsRun()
{
	if (!IsLoad())
		return false;

	switch (m_state)
	{
	case eState::Stop:
		return false;

	case eState::DebugWait:
	case eState::DebugStep:
	case eState::DebugRun:
		return true;
	default:
		assert(!"cDebugger::IsRun() not vailid state");
		return false;
	}	
}


// clear debugger information
void cDebugger::Clear()
{
	if (m_interpreter)
		m_interpreter->Clear();

	m_interpreter = nullptr;
	m_state = eState::Stop;
}
