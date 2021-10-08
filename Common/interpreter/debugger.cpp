
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

	if (!m_interpreter->LoadIntermediateCode(fileName))
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
		if (ProcessUntilNodeEnter(m_dt) == 1)
			m_state = eState::Wait;
		else
			m_state = eState::Step;
		m_dt = 0.f;
		break;
	case eState::Run:
		m_interpreter->Process(deltaSeconds);

		// check breakpoint
		if (!m_breakPoints.empty())
			if (CheckBreakPoint())
				m_state = eState::Break;
		break;
	case eState::Break:
		m_state = eState::Wait;
		break;
	default:
		assert(!"cDebugger::Process() not vailid state");
		break;
	}
	return true;
}


// process until node enter
// return 0: no meet 'node enter'
//		  1: meet 'node enter'
int cDebugger::ProcessUntilNodeEnter(const float deltaSeconds)
{
	int state = 1; // 1:loop, 2:meet node enter, 3:wait event, 4:fail
	while (state == 1)
	{
		m_interpreter->Process(deltaSeconds);

		state = 0; // initial state
		for (auto &vm : m_interpreter->m_vms)
		{
			if (vm->m_code.m_codes.size() <= (vm->m_reg.idx))
			{
				if (state == 0)
					state = 4; // fail, no running virtual machine
				continue;
			}
		
			const script::sInstruction &code = vm->m_code.m_codes[vm->m_reg.idx];
			if (code.cmd == script::eCommand::nop)
			{
				if ((state == 0) || (state == 4))
					state = 3; // wait event
				continue;
			}

			state = 1; // at least, one virtual machine running
			if (code.cmd != script::eCommand::cmt)
				continue;
			if (code.str1 == "node enter")
			{
				state = 2;
				break;
			}
		}
		break;
	}

	return (state == 2)? 1 : 0;
}


// check current instruction location has contain breakpoint
// return: true = foundbreakpoint
//		   false = not found
bool cDebugger::CheckBreakPoint()
{
	for (auto &vm : m_interpreter->m_vms)
	{
		if (vm->m_code.m_codes.size() <= vm->m_reg.idx)
			continue;
		const sInstruction &code = vm->m_code.m_codes[vm->m_reg.idx];
		if ((code.cmd != eCommand::cmt) || (code.str1 != "node enter"))
			continue;
		if (m_breakPoints.end() == m_breakPoints.find(code.reg1))
			continue;
		// find breakpoint
		return true;	
	}
	return false;
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


// cancel debugging, run interpreter, one step debugging state
bool cDebugger::StepRun()
{
	RETV(!m_interpreter, false);

	if (m_state != eState::Wait)
		return false;

	if (!m_interpreter->Run())
		return false;

	m_state = eState::Step;
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


// register break point
// id: node id
bool cDebugger::BreakPoint(const bool enable, const uint id)
{
	if (enable)
		m_breakPoints.insert(id);
	else
		m_breakPoints.erase(id);
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
	case eState::Break:
	case eState::Run:
		return true;
	default:
		assert(!"cDebugger::IsRun() not vailid state");
		return false;
	}	
}


// is debugging state?
bool cDebugger::IsDebug()
{
	if (!IsLoad())
		return false;

	switch (m_state)
	{
	case eState::Stop:
	case eState::Run:
		return false;
	case eState::Wait:
	case eState::Step:
	case eState::Break:
		return true;
	default:
		assert(!"cDebugger::IsDebug() not vailid state");
		return false;
	}
}


bool cDebugger::IsBreak()
{
	return m_state == eState::Break;
}


// clear debugger information
void cDebugger::Clear()
{
	if (m_interpreter)
		m_interpreter->Clear();

	m_interpreter = nullptr;
	m_state = eState::Stop;
	m_breakPoints.clear();
}
