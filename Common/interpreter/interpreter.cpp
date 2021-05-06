
#include "stdafx.h"
#include "interpreter.h"

using namespace common;
using namespace common::script;


cInterpreter::cInterpreter()
	: m_state(eState::Stop)
	, m_runState(eRunState::Stop)
	, m_dbgState(eDebugState::Stop)
	, m_callback(nullptr)
	, m_callbackArgPtr(nullptr)
	, m_dt(0.f)
{
}

cInterpreter::~cInterpreter()
{
	Clear();
}


// initialize interpreter
bool cInterpreter::Init(iFunctionCallback *callback
	, void *arg //= nullptr
)
{
	Clear();

	m_callback = callback;
	m_callbackArgPtr = arg;
	return true;
}


// read intermediate code
bool cInterpreter::ReadIntermediateCode(const StrPath &icodeFileName)
{
	m_fileName = icodeFileName;
	if (!m_code.Read(icodeFileName))
		return false;
	return true;
}


// process interpreter virtual machine
bool cInterpreter::Process(const float deltaSeconds)
{
	switch (m_state) 
	{
	case eState::Stop: break; // nothing~
	case eState::Run: RunProcess(deltaSeconds); break;
	case eState::DebugRun: DebugProcess(deltaSeconds); break;
	default:
		assert(0);
		break;
	}
	return true;
}


// run interpreter process
bool cInterpreter::RunProcess(const float deltaSeconds)
{
	while (!m_events.empty())
	{
		cEvent &evt = m_events.front();
		for (auto &vm : m_vms)
			vm->PushEvent(evt);
		m_events.pop();
	}

	for (auto &vm : m_vms)
		vm->Process(deltaSeconds);

	return true;
}


// debug interpreter process
bool cInterpreter::DebugProcess(const float deltaSeconds)
{
	switch (m_dbgState)
	{
	case eDebugState::Stop:
		m_dt = 0.f;
		break;
	case eDebugState::Wait:
		m_dt += deltaSeconds;
		break;
	case eDebugState::Step:
		m_dt += deltaSeconds;
		if (ProcessUntilNodeEnter(m_dt) == 1)
			m_dbgState = eDebugState::Wait;
		else
			m_dbgState = eDebugState::Step;
		m_dt = 0.f;
		break;
	case eDebugState::Run:
		RunProcess(deltaSeconds);

		// check breakpoint
		if (!m_breakPoints.empty())
			if (CheckBreakPoint())
				m_dbgState = eDebugState::Break;
		break;
	case eDebugState::Break:
		m_dbgState = eDebugState::Wait;
		break;
	default:
		assert(!"cInterpreter::DebugProcess() not vailid state");
		break;
	}
	return true;
}


// run interpreter
bool cInterpreter::Run()
{
	if (!m_code.IsLoaded())
		return false;

	m_state = eState::Run;
	InitAndRunVM();
	return true;
}


// run interpreter with debug state
// break when meet break point or force break
bool cInterpreter::DebugRun()
{
	if (!m_code.IsLoaded())
		return false;

	m_state = eState::DebugRun;
	m_dbgState = eDebugState::Run;
	InitAndRunVM();
	return true;
}


// run interpreter with debug state
// start with debug wait state
bool cInterpreter::StepRun()
{
	if (!m_code.IsLoaded())
		return false;

	m_state = eState::DebugRun;
	m_dbgState = eDebugState::Step;
	InitAndRunVM();
	return true;
}


// stop interpreter
bool cInterpreter::Stop()
{
	m_state = eState::Stop;
	for (auto &vm : m_vms)
		vm->Stop();
	return true;
}


bool cInterpreter::PushEvent(const cEvent &evt)
{
	m_events.push(evt);
	return true;
}


// resume debug
bool cInterpreter::Resume()
{
	if (m_state != eState::DebugRun)
		return false;
	if ((m_dbgState != eDebugState::Wait)
		&& (m_dbgState != eDebugState::Step))
		return false; // state error

	m_dbgState = eDebugState::Run;
	return true;
}


// one step debugging
bool cInterpreter::OneStep()
{
	if (m_state != eState::DebugRun)
		return false;
	m_dbgState = eDebugState::Step;
	return true;
}


// break interpreter running state, and then change debug state
bool cInterpreter::Break()
{
	if (m_state != eState::DebugRun)
		return false;
	m_dbgState = eDebugState::Wait;
	return true;
}


// register break point
// id: node id
bool cInterpreter::BreakPoint(const bool enable, const uint id)
{
	if (enable)
		m_breakPoints.insert(id);
	else
		m_breakPoints.erase(id);
	return true;
}


// intialize VirtualMachine and Run Intermediate Code
bool cInterpreter::InitAndRunVM()
{
	cVirtualMachine *vm = nullptr;
	if (m_vms.empty())
	{
		vm = new cVirtualMachine("VM1");
		m_vms.push_back(vm);
	}
	else
	{
		vm = m_vms.back();
	}

	if (!vm->Init(m_code, m_callback, m_callbackArgPtr))
		return false;

	vm->Run();
	return true;
}


// process until node enter
// return 0: no meet 'node enter'
//		  1: meet 'node enter'
int cInterpreter::ProcessUntilNodeEnter(const float deltaSeconds)
{
	int state = 1; // 1:loop, 2:meet node enter, 3:wait event, 4:fail
	while (state == 1)
	{
		RunProcess(deltaSeconds);

		state = 0; // initial state
		for (auto &vm : m_vms)
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

	return (state == 2) ? 1 : 0;
}


// check current instruction location has contain breakpoint
// return: true = foundbreakpoint
//		   false = not found
bool cInterpreter::CheckBreakPoint()
{
	for (auto &vm : m_vms)
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


bool cInterpreter::IsRun() const { return eState::Run == m_state; }
bool cInterpreter::IsStop() const { return eState::Stop == m_state; }


// is debugging state?
bool cInterpreter::IsDebug()
{
	switch (m_state)
	{
	case eState::Stop:
	case eState::Run:
		return false;
	case eState::DebugRun: // check next code
	default:
		break;
	}

	switch (m_dbgState)
	{
	case eDebugState::Stop:
	case eDebugState::Run:
		return false;
	case eDebugState::Wait:
	case eDebugState::Step:
	case eDebugState::Break:
		return true;
	default:
		assert(!"cInterpreter::IsDebug() not vailid state");
		return false;
	}
}


bool cInterpreter::IsBreak()
{
	return (m_state == eState::DebugRun) && (m_dbgState == eDebugState::Break);
}


void cInterpreter::Clear()
{
	Stop();

	for (auto &vm : m_vms)
	{
		vm->Clear();
		delete vm;
	}
	m_vms.clear();

	m_code.Clear();
	while (!m_events.empty())
		m_events.pop();
}
