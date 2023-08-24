
#include "stdafx.h"
#include "interpreter.h"

using namespace common;
using namespace common::script;


cInterpreter::cInterpreter()
	: m_state(eState::Stop)
	, m_runState(eRunState::Stop)
	, m_dbgState(eDebugState::Stop)
	, m_dbgVmId(-1)
	, m_dt(0.f)
	, m_isCodeTrace(false)
	, m_isICodeStep(false)
	, m_listener(nullptr)
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


// load intermediate code, add new vm
// parentVmId: parent virtual machine id, -1:root
// return virtual machine id, -1: error
int cInterpreter::LoadIntermediateCode(const StrPath &icodeFileName
	, const int parentVmId //=-1
	, const string& scopeName //= ""
)
{
	cIntermediateCode icode;
	if (!icode.Read(icodeFileName))
		return -1;
	return InitVM(icode, parentVmId, scopeName);
}


// load intermediate code, deep copy, add new vm
// parentVmId: parent virtual machine id, -1:root
// return virtual machine id, -1: error
int cInterpreter::LoadIntermediateCode(const cIntermediateCode &icode
	, const int parentVmId //=-1
	, const string& scopeName //= ""
)
{
	return InitVM(icode, parentVmId, scopeName);
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

	// remove vm
	if (!m_rmVms.empty())
	{
		// remove child first
		for (auto &it = m_rmVms.rbegin(); it != m_rmVms.rend(); ++it)
		{
			auto& vm = *it;
			if (m_listener)
				m_listener->TerminateResponse(vm->m_id);
			vm->Clear();
			delete vm;
		}
		m_rmVms.clear();
	}
	//~

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

	for (uint i = 0; i < m_vms.size(); ++i)
	{
		cVirtualMachine *vm = m_vms[i];
		vm->Process(deltaSeconds);
	}

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
			if (ProcessUntilNodeEnter(m_dbgVmId, m_dt) == 1)
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
// vmId: virtual machine id, -1:all vm
// symbTable: execute argument
bool cInterpreter::Run(const int vmId
	, const script::cSymbolTable& symbTable //= {}
	, const string& startEvent //= "Start Event"
)
{
	if (m_vms.empty())
		return false;

	if (vmId < 0)
	{
		m_state = eState::Run;
	}
	else
	{
		if (eState::Stop == m_state)
			m_state = eState::Run;
	}
	return RunVM(vmId, symbTable, startEvent);
}


// run interpreter with debug state
// break when meet break point or force break
// vmId: virtual machine id, -1:all vm
// symbTable: execute argument
bool cInterpreter::DebugRun(const int vmId
	, const script::cSymbolTable& symbTable //= {}
	, const string& startEvent //= "Start Event"
)
{
	if (m_vms.empty())
		return false;

	if (vmId < 0)
	{
		m_state = eState::DebugRun;
		m_dbgState = eDebugState::Run;
	}
	else
	{
		if (eState::Stop == m_state)
		{
			m_state = eState::DebugRun;
			m_dbgState = eDebugState::Run;
		}
	}
	return RunVM(vmId, symbTable, startEvent);
}


// run interpreter with debug state
// start with debug wait state
// vmId: virtual machine id, -1:all vm
// symbTable: execute argument
bool cInterpreter::StepRun(const int vmId
	, const script::cSymbolTable& symbTable //= {}
	, const string& startEvent //= "Start Event"
)
{
	if (m_vms.empty())
		return false;

	if (vmId < 0)
	{
		m_state = eState::DebugRun;
		m_dbgState = eDebugState::Step;
	}
	else
	{
		if (eState::Stop == m_state)
		{
			m_state = eState::DebugRun;
			m_dbgState = eDebugState::Step;
		}
	}
	return RunVM(vmId, symbTable, startEvent);
}


// stop interpreter
// vmId: virtual machine id, -1:all vm
bool cInterpreter::Stop(const int vmId)
{
	if (vmId < 0)
	{
		m_state = eState::Stop;
		for (auto &vm : m_vms)
			vm->Stop();
	}
	else
	{
		cVirtualMachine* vm = GetVM(vmId);
		if (vm)
			vm->Stop();
	}
	m_dbgVmId = -1;
	return true;
}


// terminate virtual machine
// vmId: terminate virtual machine id
// return: success?
bool cInterpreter::Terminate(const int vmId
	, const script::cSymbolTable& symbTable //= {}
	, const string& eventName //= "_Exit"
)
{
	// invoke XXX_eventName event, contain 'return value' with Terminate 'args' data
	cVirtualMachine* vm = GetVM(vmId);
	if (!vm)
		return false;
	if ((vm->m_parentId >= 0) && !vm->m_scopeName.empty())
	{
		cVirtualMachine* parent = GetVM(vm->m_parentId);
		if (parent)
		{
			script::cEvent evt(vm->m_scopeName + eventName);

			if (eventName == "_Exit")
			{
				for (auto& kv : symbTable.m_vars)
					for (auto& var : kv.second)
						parent->m_symbTable.m_vars[vm->m_scopeName]["return value"] = var.second;
			}
			else
			{
				for (auto& kv : symbTable.m_vars)
					for (auto& var : kv.second)
						parent->m_symbTable.m_vars[vm->m_scopeName][var.first] = var.second;
			}
			parent->PushEvent(evt);
		}
	}
	//~

	// remove all child vm
	queue<int> q;
	q.push(vmId);
	while (!q.empty())
	{
		const int vid = q.front();
		q.pop();
		for (auto& v : m_vms)
		{
			if (vid == v->m_parentId)
				q.push(v->m_id);
		}

		// cannot remove root vm (parentId:-1)
		cVirtualMachine* v = GetVM(vid);
		if (vm && (-1 != v->m_parentId))
		{
			common::removevector(m_vms, v);
			m_rmVms.push_back(v);
		}
	}
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


// set debug virtual machine
// vmId: -1=no debug
void cInterpreter::SetDebugVM(const int vmId)
{
	m_dbgVmId = vmId;
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
// vmId: virtual machine id, -1:all vm
bool cInterpreter::Resume(const int vmId)
{
	if (m_state != eState::DebugRun)
		return false;
	if ((m_dbgState != eDebugState::Wait)
		&& (m_dbgState != eDebugState::Step))
		return false; // state error
	if (vmId >= 0)
		return false; // only all vm, must -1

	m_dbgState = eDebugState::Run;
	return true;
}


// one step debugging
// vmId: virtual machine id, -1:all vm
bool cInterpreter::OneStep(const int vmId)
{
	if (m_state != eState::DebugRun)
		return false;
	m_dbgState = eDebugState::Step;
	return true;
}


// break interpreter running state, and then change debug state
// vmId: virtual machine id, -1:all vm
bool cInterpreter::Break(const int vmId)
{
	if (m_state != eState::DebugRun)
		return false;
	m_dbgState = eDebugState::Wait;
	return true;
}


// register break point
// id: node id
// vmId: virtual machine id, -1:all vm
bool cInterpreter::BreakPoint(const bool enable, const int vmId, const uint id)
{
	if (enable)
		m_breakPoints.insert({ vmId, id });
	else
		m_breakPoints.erase({ vmId, id });
	return true;
}


// intialize VirtualMachine and Run Intermediate Code
// vmId: virtual machine id, -1:all vm
// symbTable: execute argument
bool cInterpreter::RunVM(const int vmId
	, const script::cSymbolTable& symbTable //= {}
	, const string& startEvent //= "Start Event"
)
{
	if (vmId < 0)
	{
		for (auto& vm : m_vms)
		{
			for (auto& mod : m_modules)
				vm->AddModule(mod);
			if (vm->Run())
			{
				vm->PushEvent(cEvent(startEvent)); // invoke 'Start Event'
				for (auto& kv : symbTable.m_vars)
					for (auto& var : kv.second)
						vm->m_symbTable.m_vars[startEvent][var.first] = var.second;
				for (auto& kv : symbTable.m_varMap)
					vm->m_symbTable.m_varMap[kv.first] = kv.second;
			}
		}
	}
	else
	{
		cVirtualMachine *vm = GetVM(vmId);
		if (!vm) 
			return false; // error return
		for (auto &mod : m_modules)
			vm->AddModule(mod);
		if (vm->Run())
		{
			vm->PushEvent(cEvent(startEvent)); // invoke 'Start Event'
			for (auto& kv : symbTable.m_vars)
				for (auto& var : kv.second)
					vm->m_symbTable.m_vars[startEvent][var.first] = var.second;
			for (auto& kv : symbTable.m_varMap)
				vm->m_symbTable.m_varMap[kv.first] = kv.second;
		}
	}
	return true;
}


// initialize virtual machine
// parentVmId: parent virtual machine id, -1:root
// return virtual machine id, -1: error
int cInterpreter::InitVM(const cIntermediateCode& icode
	, const int parentVmId //=-1
	, const string& scopeName //= ""
)
{
	string vmName;
	if (icode.m_fileName.empty())
	{
		vmName = common::format("VM%d", m_vms.size());
	}
	else
	{
		vmName = icode.m_fileName.GetFileNameExceptExt().c_str();
		vmName += common::format(".VM%d", m_vms.size());
	}

	cVirtualMachine* vm = new cVirtualMachine(vmName);
	vm->SetCodeTrace(m_isCodeTrace);
	if (!vm->Init(this, icode, parentVmId, scopeName))
	{
		delete vm;
		return -1;
	}
	m_vms.push_back(vm);
	return vm->m_id;
}


// process until node enter
// return 0: no meet 'node enter'
//		  1: meet 'node enter'
int cInterpreter::ProcessUntilNodeEnter(const int vmId, const float deltaSeconds)
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
			if ((vmId >= 0) && (vm->m_id != vmId))
				continue; // ignore this vm

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
	Stop(-1);

	m_dbgVmId = -1;
	for (auto &vm : m_vms)
	{
		vm->Clear();
		delete vm;
	}
	m_vms.clear();

	for (auto& vm : m_rmVms)
	{
		vm->Clear();
		delete vm;
	}
	m_rmVms.clear();

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


// return remove virtual machine specific id
cVirtualMachine* cInterpreter::GetRemoveVM(const int vmId)
{
	auto it = std::find_if(m_rmVms.begin(), m_rmVms.end()
		, [&](const auto& a) { return a->m_id == vmId; });
	if (m_rmVms.end() == it)
		return nullptr;
	return *it;
}


// update terminate virtual machine callback function
bool cInterpreter::SetListener(iTerminateResponse* listener)
{
	m_listener = listener;
	return true;
}
