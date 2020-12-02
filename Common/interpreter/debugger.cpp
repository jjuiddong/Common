
#include "stdafx.h"
#include "debugger.h"


using namespace common;
using namespace common::script;


cDebugger::cDebugger(cInterpreter *interpreter //= nullptr
)
	: m_interpreter(nullptr)
	, m_state(eState::Stop)
	, m_dt(0.f)
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

	m_state = eState::Wait;
	return true;
}


// process debugger
bool cDebugger::Process(const float deltaSeconds)
{
	RETV(!m_interpreter, false);

	switch (m_state)
	{
	case eState::Stop:
		m_dt = 0.f;
		break;
	case eState::Wait:
		m_dt += deltaSeconds;
		break;
	case eState::Step:
		m_dt += deltaSeconds;
		m_interpreter->Process(m_dt);
		m_state = eState::Wait;
		m_dt = 0.f;
		break;
	case eState::Run:
		m_interpreter->Process(deltaSeconds);
		break;
	default:
		assert(!"cDebugger::Process() not vailid state");
		break;
	}
	return true;
}


// cancel debugging, run interpreter
bool cDebugger::Run()
{
	RETV(!m_interpreter, false);

	if (m_state != eState::Wait)
		return false;

	if (!m_interpreter->Run())
		return false;

	m_state = eState::Run;
	return true;
}


// stop debugging state
bool cDebugger::Stop()
{
	RETV(!m_interpreter, false);

	m_interpreter->Stop(); // stop state
	m_state = eState::Stop;
	return true;
}


// resume debug run
bool cDebugger::Resume()
{
	RETV(!m_interpreter, false);

	if ((m_state != eState::Wait)
		&& (m_state != eState::Step))
		return false; // state error

	m_state = eState::Run;
	return true;
}


// one step debugging
bool cDebugger::OneStep()
{
	RETV(!m_interpreter, false);
	m_state = eState::Step;
	return true;
}


// break interpreter running state, and then change debug state
bool cDebugger::Break()
{
	RETV(!m_interpreter, false);
	m_state = eState::Wait;
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

	case eState::Wait:
	case eState::Step:
	case eState::Run:
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
