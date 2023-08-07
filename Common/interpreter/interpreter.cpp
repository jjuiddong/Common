
#include "stdafx.h"
#include "interpreter.h"

using namespace common;
using namespace common::script;


cInterpreter::cInterpreter()
	: m_state(eState::Stop)
	, m_runState(eRunState::Stop)
	, m_dbgState(eDebugState::Stop)
	, m_dt(0.f)
	, m_isCodeTrace(false)
	, m_isICodeStep(false)
{
}

cInterpreter::~cInterpreter()
{
	Clear();
}


// initialize interpreter
bool cInterpreter::Init()
{
	Clear();
	return true;
}


// load intermediate code
bool cInterpreter::LoadIntermediateCode(const StrPath &icodeFileName)
{
	m_fileName = icodeFileName;
	cIntermediateCode icode;
	if (!icode.Read(icodeFileName))
		return false;
	InitVM(icode);
	return true;
}


// load intermediate code, deep copy
bool cInterpreter::LoadIntermediateCode(const cIntermediateCode &icode)
{
	m_fileName = icode.m_fileName;
	InitVM(icode);
	return true;
}


// add function execute module
bool cInterpreter::AddModule(iModule *mod)
{
	auto it = std::find(m_modules.begin(), m_modules.end(), mod);
	if (m_modules.end() != it)
		return false; // already exist
	m_modules.push_back(mod);
	return true;
}


// remove function execute module
bool cInterpreter::RemoveModule(iModule *mod)
{
	common::removevector(m_modules, mod);
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
		for (auto& vm : m_vms)
			if ((evt.m_vmId < 0) || (evt.m_vmId == vm->m_id))
				vm->PushEvent(evt);
		m_events.pop_front();
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
		if (m_isICodeStep)
		{
			// one icode execution
			RunProcess(m_dt);
			m_dbgState = eDebugState::Wait;
		}
		else
		{
			if (ProcessUntilNodeEnter(m_dt) == 1)
				m_dbgState = eDebugState::Wait;
			else
				m_dbgState = eDebugState::Step;
		}
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
	if (m_vms.empty())
		return false;

	m_state = eState::Run;
	RunVM();
	return true;
}


// run interpreter with debug state
// break when meet break point or force break
bool cInterpreter::DebugRun()
{
	if (m_vms.empty())
		return false;

	m_state = eState::DebugRun;
	m_dbgState = eDebugState::Run;
	RunVM();
	return true;
}


// run interpreter with debug state
// start with debug wait state
bool cInterpreter::StepRun()
{
	if (m_vms.empty())
		return false;

	m_state = eState::DebugRun;
	m_dbgState = eDebugState::Step;
	RunVM();
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


// add event
// vmId: virtual machine id, -1: all vm
bool cInterpreter::PushEvent(const int vmId, const cEvent &evt)
{
	m_events.push_back(evt);
	m_events.back().m_vmId = vmId; // update vmId
	return true;
}


// set virtual machine instruction index trace for debugging
void cInterpreter::SetCodeTrace(const bool isTrace)
{
	m_isCodeTrace = isTrace;
	for (auto &vm : m_vms)
		vm->SetCodeTrace(isTrace);
}


// set one intermediate code step debugging
void cInterpreter::SetICodeStepDebug(const bool isICodeStep)
{
	m_isICodeStep = isICodeStep;
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


// resume virtual machine if WaitCallback state
bool cInterpreter::ResumeVM(const string &vmName)
{
	for (auto &vm : m_vms)
		if (vm->m_name == vmName)
			return vm->Resume();
	return false;
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
bool cInterpreter::BreakPoint(const bool enable, const int vmId, const uint id)
{
	if (enable)
		m_breakPoints.insert({ vmId, id });
	else
		m_breakPoints.erase({ vmId, id });
	return true;
}


// intialize VirtualMachine and Run Intermediate Code
bool cInterpreter::RunVM()
{
	if (m_vms.empty())
		return false;

	cVirtualMachine* vm = m_vms.back();
	for (auto &mod : m_modules)
		vm->AddModule(mod);

	vm->Run();
	vm->PushEvent(cEvent("Start Event")); // invoke 'Start Event'
	vm->m_isStartEvt = true;
	return true;
}


// initialize virtual machine
bool cInterpreter::InitVM(const cIntermediateCode& icode)
{
	cVirtualMachine* vm = nullptr;
	if (m_vms.empty())
	{
		string vmName;
		if (icode.m_fileName.empty())
		{
			vmName = "VM1";
		}
		else
		{
			vmName = icode.m_fileName.GetFileNameExceptExt().c_str();
			vmName += ".VM1";
		}

		vm = new cVirtualMachine(vmName);
		vm->SetCodeTrace(m_isCodeTrace);
		m_vms.push_back(vm);
	}
	else
	{
		vm = m_vms.back();
	}

	if (!vm->Init(icode))
		return false;

	return true;
}


// process until node enter
// return 0: no meet 'node enter'
//		  1: meet 'node enter'
int cInterpreter::ProcessUntilNodeEnter(const float deltaSeconds)
{
	float dt = deltaSeconds;
	int state = 1; // 1:loop, 2:meet node enter, 3:wait event, 4:fail
	while (state == 1)
	{
		RunProcess(dt);
		dt = 0.f;

		// node debugging
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
		if (m_breakPoints.end() == m_breakPoints.find({ vm->m_id, code.reg1 }))
			continue;
		// find breakpoint
		return true;
	}
	return false;
}


bool cInterpreter::IsRun() const { return eState::Run == m_state; }
bool cInterpreter::IsDebugRun() const { return eState::DebugRun == m_state; }
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

	m_events.clear();
	m_modules.clear();
}


// return virtual machine specific id
cVirtualMachine* cInterpreter::GetVM(const int vmId)
{
	auto it = std::find_if(m_vms.begin(), m_vms.end()
		, [&](const auto& a) { return a->m_id == vmId; });
	if (m_vms.end() == it)
		return nullptr;
	return *it;
}
