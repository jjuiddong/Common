
#include "stdafx.h"
#include "virtualmachine.h"

using namespace common;
using namespace common::script;


cVirtualMachine::cVirtualMachine(const StrId &name)
	: m_state(eState::Stop)
	, m_callback(nullptr)
	, m_callbackArgPtr(nullptr)
	, m_name(name)
{
}

cVirtualMachine::~cVirtualMachine()
{
	Clear();
}


// initialize virtual machine
bool cVirtualMachine::Init(const cIntermediateCode &code, iFunctionCallback *callback
	, void *arg //= nullptr
)
{
	Clear();

	m_reg.idx = 0;
	m_reg.exeIdx = 0;
	m_reg.cmp = false;

	m_code = code;
	m_symbTable = code.m_variables; // initial variable
	m_callback = callback;
	m_callbackArgPtr = arg;

	// update timer
	for (auto time : m_code.m_timer1Events)
	{
		m_timers.push_back(
			{ time.first // timer name
			, time.second / 1000.f // timer interval (convert seconds unit)
			, (float)time.second / 1000.f } // timer decrease time (convert seconds unit)
		);
	}

	m_stack.reserve(32);
	return true;
}


// execute event, instruction
bool cVirtualMachine::Process(const float deltaSeconds)
{
	RETV(eState::Stop == m_state, true);
	RETV(!m_code.IsLoaded(), true);

	if (!ProcessEvent(deltaSeconds))
		ProcessTimer(deltaSeconds);
	ExecuteInstruction(deltaSeconds, m_reg);

	return true;
}


// start virtual machine
bool cVirtualMachine::Run()
{
	RETV(eState::Run == m_state, false); // already run

	m_state = eState::Run;

	// jump to entry point
	auto it = m_code.m_jmpMap.find("main");
	if (m_code.m_jmpMap.end() != it)
		m_reg.idx = it->second + 1; // +1 next to label command

	m_reg.cmp = false;

	return true;
}


// stop virtual machine
bool cVirtualMachine::Stop()
{
	m_state = eState::Stop;
	m_reg.idx = UINT_MAX;
	m_reg.exeIdx = UINT_MAX;

	// clear event, timer, stack
	while (!m_events.empty())
		m_events.pop();
	m_timers.clear();
	m_stack.clear();
	return true;
}


bool cVirtualMachine::PushEvent(const cEvent &evt)
{
	m_events.push(evt);
	return true;
}


// process event
// execute event only waitting state
// waitting state is nop instruction state
bool cVirtualMachine::ProcessEvent(const float deltaSeconds)
{
	RETV(eState::Wait != m_state, false);
	RETV(m_events.empty(), false);

	auto &evt = m_events.front();
	const uint addr = m_code.FindJumpAddress(evt.m_name);
	if (UINT_MAX != addr)
	{
		// update symboltable
		for (auto &kv : evt.m_vars)
		{
			vector<string> out;
			common::tokenizer(kv.first.c_str(), "::", "", out);
			if (out.size() >= 2)
				m_symbTable.Set(out[0].c_str(), out[1].c_str(), kv.second);
		}
		m_reg.idx = addr; // jump instruction code
		m_state = eState::Run;
	}
	else
	{
		// error occurred!!
		// not found event handling
		dbg::Logc(1, "cVirtualMachine::Update(), Not Found EventHandling evt:%s \n"
			, evt.m_name.c_str());
	}

	m_events.pop();

	return true;
}


// process timer
// check timer interval time, call timer event
// execute event only waitting state
bool cVirtualMachine::ProcessTimer(const float deltaSeconds)
{
	RETV(eState::Wait != m_state, false);
	RETV(m_timers.empty(), false);

	for (auto &time : m_timers)
	{
		time.t -= deltaSeconds;
		if (time.t < 0.f)
		{
			PushEvent(cEvent(time.name));
			time.t = time.interval;
			break;
		}
	}

	return true;
}


// execute intermediate code instruction
bool cVirtualMachine::ExecuteInstruction(const float deltaSeconds, sRegister &reg)
{
	if (m_code.m_codes.size() <= reg.idx)
		return false; // end of code

	reg.exeIdx = reg.idx;
	sInstruction &code = m_code.m_codes[reg.idx];
	const VARTYPE varType = GetVarType(code.cmd); // variant type

	switch (code.cmd)
	{
	case eCommand::ldbc:
	case eCommand::ldic:
	case eCommand::ldfc:
	case eCommand::ldsc:
	case eCommand::ldac:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (varType != code.var1.vt)
			goto $error_semantic;
		reg.val[code.reg1] = code.var1;
		++reg.idx;
		break;

	case eCommand::ldcmp:
	case eCommand::ldncmp:
	{
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		const bool cmp = (code.cmd == eCommand::ldncmp) ? !reg.cmp : reg.cmp;
		reg.val[code.reg1] = cmp ? variant_t((bool)true) : variant_t((bool)false);
		++reg.idx;
	}
	break;

	case eCommand::ldtim:
	{
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		reg.tim = (float)reg.val[code.reg1];
		++reg.idx;
	}
	break;

	case eCommand::getb:
	case eCommand::geti:
	case eCommand::getf:
	case eCommand::gets:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (m_symbTable.Get(code.str1, code.str2, reg.val[code.reg1]))
		{
			if (varType != reg.val[code.reg1].vt)
				goto $error_semantic;
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::geta:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (m_symbTable.Get(code.str1, code.str2, reg.val[code.reg1]))
		{
			const bool isArrayType = (reg.val[code.reg1].vt & VT_BYREF); // VT_ARRAY? ticky code
			if (!isArrayType)
				goto $error_semantic;
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::getm:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (m_symbTable.Get(code.str1, code.str2, reg.val[code.reg1]))
		{
			// ticky code, check map type 
			const bool isMapType = (reg.val[code.reg1].vt & VT_BYREF);
			if (!isMapType)
				goto $error_semantic;
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::setb:
	case eCommand::seti:
	case eCommand::setf:
	case eCommand::sets:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (m_symbTable.Set(code.str1, code.str2, reg.val[code.reg1]))
		{
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::seta:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (!(reg.val[code.reg1].vt & VT_BYREF)) //VT_ARRAY? (tricky code)
			goto $error_semantic;
		if (m_symbTable.Set(code.str1, code.str2, reg.val[code.reg1]))
		{
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::setm:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		// tricky code, check map type
		if (!(reg.val[code.reg1].vt & VT_BYREF))
			goto $error_semantic;
		if (m_symbTable.Set(code.str1, code.str2, reg.val[code.reg1]))
		{
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::copya:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (!(reg.val[code.reg1].vt & VT_BYREF)) //VT_ARRAY? (tricky code)
			goto $error_semantic;
		if (m_symbTable.CopyArray(code.str1, code.str2, reg.val[code.reg1]))
		{
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::copym:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		// tricky code, check map type
		if (!(reg.val[code.reg1].vt & VT_BYREF))
			goto $error_semantic;
		if (m_symbTable.CopyMap(code.str1, code.str2, reg.val[code.reg1]))
		{
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::addi:
	case eCommand::subi:
	case eCommand::muli:
	case eCommand::divi:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;
		switch (code.cmd)
		{
		case eCommand::addi: reg.val[9] = reg.val[code.reg1].intVal + reg.val[code.reg2].intVal; break;
		case eCommand::subi: reg.val[9] = reg.val[code.reg1].intVal - reg.val[code.reg2].intVal; break;
		case eCommand::muli: reg.val[9] = reg.val[code.reg1].intVal * reg.val[code.reg2].intVal; break;
		case eCommand::divi: reg.val[9] = reg.val[code.reg1].intVal / reg.val[code.reg2].intVal; break;
		}		
		++reg.idx;
		break;

	case eCommand::addf:
	case eCommand::subf:
	case eCommand::mulf:
	case eCommand::divf:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;
		switch (code.cmd)
		{
		case eCommand::addf: reg.val[9] = reg.val[code.reg1].fltVal + reg.val[code.reg2].fltVal; break;
		case eCommand::subf: reg.val[9] = reg.val[code.reg1].fltVal - reg.val[code.reg2].fltVal; break;
		case eCommand::mulf: reg.val[9] = reg.val[code.reg1].fltVal * reg.val[code.reg2].fltVal; break;
		case eCommand::divf: reg.val[9] = reg.val[code.reg1].fltVal / reg.val[code.reg2].fltVal; break;
		}
		++reg.idx;
		break;

	case eCommand::negate:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		reg.val[9] = !((bool)reg.val[code.reg1]);
		++reg.idx;
		break;

	case eCommand::adds:
	{
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;
		const string s0 = variant2str(reg.val[code.reg1]);
		const string s1 = variant2str(reg.val[code.reg2]);
		reg.val[9] = (s0 + s1).c_str();
		++reg.idx;
	}
	break;

	case eCommand::eqi:
	case eCommand::eqf:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (reg.val[code.reg1].vt == VT_BOOL)
		{
			// force converting int
			reg.val[code.reg1] = ((bool)reg.val[code.reg1]) ? 1 : 0;
		}
		if (reg.val[code.reg2].vt == VT_BOOL)
		{
			// force converting int
			reg.val[code.reg2] = ((bool)reg.val[code.reg2]) ? 1 : 0;
		}
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1] == reg.val[code.reg2]);
		++reg.idx;
		break;

	case eCommand::eqs:
	{
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;

		const string str1 = common::variant2str(reg.val[code.reg1]);
		const string str2 = common::variant2str(reg.val[code.reg2]);
		reg.cmp = (str1 == str2);
		++reg.idx;
	}
	break;

	case eCommand::eqic:
	case eCommand::eqfc:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (reg.val[code.reg1].vt == VT_BOOL)
		{
			// force converting integer
			reg.val[code.reg1] = ((bool)reg.val[code.reg1]) ? 1 : 0;
		}
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != code.var1.vt)
			goto $error_semantic;

		reg.cmp = reg.val[code.reg1] == code.var1;
		++reg.idx;
		break;

	case eCommand::eqsc:
	{
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != code.var1.vt)
			goto $error_semantic;

		reg.cmp = reg.val[code.reg1] == code.var1;
		++reg.idx;
	}
	break;


	case eCommand::lesi:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].intVal < reg.val[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::lesf:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].fltVal < reg.val[code.reg2].fltVal);
		++reg.idx;
		break;

	case eCommand::leqi:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].intVal <= reg.val[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::leqf:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].fltVal <= reg.val[code.reg2].fltVal);
		++reg.idx;
		break;

	case eCommand::gri:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].intVal > reg.val[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::grf:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].fltVal > reg.val[code.reg2].fltVal);
		++reg.idx;
		break;

	case eCommand::greqi:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].intVal >= reg.val[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::greqf:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (varType != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].fltVal >= reg.val[code.reg2].fltVal);
		++reg.idx;
		break;

	case eCommand::jnz:
		// jump, if reg.cmp true
		if (reg.cmp)
		{
			auto it = m_code.m_jmpMap.find(code.str1);
			if (m_code.m_jmpMap.end() == it)
				goto $error; // not found jump address
			reg.idx = it->second; // jump instruction code
		}
		else
		{
			++reg.idx;
		}
		break;

	case eCommand::jmp:
	{
		// jump, label code.str1
		auto it = m_code.m_jmpMap.find(code.str1);
		if (m_code.m_jmpMap.end() == it)
			goto $error; // not found jump address
		reg.idx = it->second; // jump instruction code
	}
	break;

	case eCommand::call:
	{
		if (!m_callback)
			goto $error;
		
		string funcName;
		int nodeId;
		std::tie(funcName, nodeId) = cSymbolTable::ParseScopeName(code.str1);
		if (funcName.empty())
			goto $error;

		m_callback->Function(m_symbTable, code.str1, funcName, m_callbackArgPtr);
		++reg.idx;
	}
	break;

	case eCommand::pushic:
	{
		if (varType != code.var1.vt)
			goto $error_semantic;
		m_stack.push_back((int)code.var1);
		++reg.idx;
	}
	break;

	case eCommand::pop:
	{
		if (!m_stack.empty())
			m_stack.pop_back();
		++reg.idx;
	}
	break;

	case eCommand::sret:
	{
		if (m_stack.empty())
		{
			// no target to jump?, next instruction
			++reg.idx;
		}
		else
		{
			// no pop stack
			const int idx = m_stack.back();
			if (m_code.m_codes.size() > (uint)idx)
				reg.idx = idx; // jump instruction index
		}
	}
	break;

	case eCommand::cstack:
	{
		m_stack.clear();
		++reg.idx;
	}
	break;

	case eCommand::delay:
		reg.tim -= (deltaSeconds * 1000.f); // second -> millisecond unit
		if (reg.tim < 0.f)
			++reg.idx; // goto next instruction
		break;

	case eCommand::label:
	case eCommand::cmt:
		++reg.idx; // no operation
		break; 

	case eCommand::nop:
		// no operation, change wait state
		m_state = eState::Wait;
		break;

	default:
		goto $error;
	}

	return true;


$error:
	dbg::Logc(3, "Error cVirtualMachine::Execute() not valid command. index=%d, cmd=%d\n"
		, reg.idx, (int)code.cmd);
	return false;

$error_semantic:
	dbg::Logc(3, "Error cVirtualMachine::Execute() Semantic Error. index=%d, cmd=%d\n"
		, reg.idx, (int)code.cmd);
	return false;

$error_memory:
	dbg::Logc(3, "Error cVirtualMachine::Execute() Memory Error. index=%d, type=%d, reg1=%d, reg2=%d\n"
		, reg.idx, (int)code.cmd, code.reg1, code.reg2);
	return false;
}


void cVirtualMachine::Clear()
{
	Stop();
	m_symbTable.Clear();
	m_code.Clear();
	while (!m_events.empty())
		m_events.pop();
	m_timers.clear();
	m_stack.clear();
}
