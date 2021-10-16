
#include "stdafx.h"
#include "virtualmachine.h"

using namespace common;
using namespace common::script;


cVirtualMachine::cVirtualMachine(const string &name)
	: m_id(common::GenerateId())
	, m_state(eState::Stop)
	, m_callback(nullptr)
	, m_callbackArgPtr(nullptr)
	, m_name(name)
	, m_isCodeTraceLog(false)
	, m_isDebugging(false)
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
			{ common::GenerateId()
			, time.first // timer name
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
	RETV((eState::Stop == m_state) || (eState::WaitCallback == m_state), true);
	RETV(!m_code.IsLoaded(), true);

	if (!ProcessEvent(deltaSeconds))
		ProcessTimer(deltaSeconds);

	ExecuteInstruction(deltaSeconds, m_reg);

	if (m_isCodeTraceLog)
		CodeTraceLog(m_trace);

	if (m_isDebugging)
		CodeTraceLog(m_trace2);

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


// resume process
bool cVirtualMachine::Resume()
{
	RETV(eState::WaitCallback != m_state, false);

	m_state = eState::Run;
	++m_reg.idx; // goto next instruction
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


// stop timer
bool cVirtualMachine::StopTimer(const int timerId)
{
	int idx = -1;
	for (uint i = 0; i < m_timers.size(); ++i)
		if (m_timers[i].id == timerId)
		{
			idx = (int)i;
			break;
		}
	if (idx < 0)
		return false;
	//common::rotatepopvector(m_timers, idx);
	common::removevector2(m_timers, idx);
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
		//dbg::Logc(1, "cVirtualMachine::Update(), Not Found EventHandling evt:%s \n"
		//	, evt.m_name.c_str());
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

	for (auto &timer : m_timers)
	{
		timer.t -= deltaSeconds;
		if (timer.t < 0.f)
		{
			// timer event trigger
			// timer id output
			const string scopeName = (timer.name + "::id").c_str();
			PushEvent(cEvent(timer.name, { {scopeName, timer.id} }));
			timer.t = timer.interval;
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
	case eCommand::ldmc:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (varType != code.var1.vt)
			goto $error_semantic;
		reg.reg[code.reg1] = code.var1;
		++reg.idx;
		break;

	case eCommand::ldcmp:
	case eCommand::ldncmp:
	{
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		const bool cmp = (code.cmd == eCommand::ldncmp) ? !reg.cmp : reg.cmp;
		reg.reg[code.reg1] = cmp ? variant_t((bool)true) : variant_t((bool)false);
		++reg.idx;
	}
	break;

	case eCommand::ldtim:
	{
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		reg.tim = (float)reg.reg[code.reg1];
		++reg.idx;
	}
	break;

	case eCommand::getb:
	case eCommand::geti:
	case eCommand::getf:
	case eCommand::gets:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (m_symbTable.Get(code.str1, code.str2, reg.reg[code.reg1]))
		{
			if (varType != reg.reg[code.reg1].vt)
				goto $error_semantic;
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::geta:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (m_symbTable.Get(code.str1, code.str2, reg.reg[code.reg1]))
		{
			const bool isArrayType = (reg.reg[code.reg1].vt & VT_BYREF); // VT_ARRAY? ticky code
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
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (m_symbTable.Get(code.str1, code.str2, reg.reg[code.reg1]))
		{
			// ticky code, check map type 
			const bool isMapType = (reg.reg[code.reg1].vt & VT_BYREF);
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
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (m_symbTable.Set(code.str1, code.str2, reg.reg[code.reg1]))
		{
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::seta:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (!(reg.reg[code.reg1].vt & VT_BYREF)) //VT_ARRAY? (tricky code)
			goto $error_semantic;
		if (m_symbTable.Set(code.str1, code.str2, reg.reg[code.reg1]))
		{
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::setm:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		// tricky code, check map type
		if (!(reg.reg[code.reg1].vt & VT_BYREF))
			goto $error_semantic;
		if (m_symbTable.Set(code.str1, code.str2, reg.reg[code.reg1]))
		{
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::copya:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (!(reg.reg[code.reg1].vt & VT_BYREF)) //VT_ARRAY? (tricky code)
			goto $error_semantic;
		if (m_symbTable.CopyArray(code.str1, code.str2, reg.reg[code.reg1]))
		{
			++reg.idx;
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::copym:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		// tricky code, check map type
		if (!(reg.reg[code.reg1].vt & VT_BYREF))
			goto $error_semantic;
		if (m_symbTable.CopyMap(code.str1, code.str2, reg.reg[code.reg1]))
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
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;
		switch (code.cmd)
		{
		case eCommand::addi: reg.reg[9] = reg.reg[code.reg1].intVal + reg.reg[code.reg2].intVal; break;
		case eCommand::subi: reg.reg[9] = reg.reg[code.reg1].intVal - reg.reg[code.reg2].intVal; break;
		case eCommand::muli: reg.reg[9] = reg.reg[code.reg1].intVal * reg.reg[code.reg2].intVal; break;
		case eCommand::divi: reg.reg[9] = reg.reg[code.reg1].intVal / reg.reg[code.reg2].intVal; break;
		}		
		++reg.idx;
		break;

	case eCommand::addf:
	case eCommand::subf:
	case eCommand::mulf:
	case eCommand::divf:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;
		switch (code.cmd)
		{
		case eCommand::addf: reg.reg[9] = reg.reg[code.reg1].fltVal + reg.reg[code.reg2].fltVal; break;
		case eCommand::subf: reg.reg[9] = reg.reg[code.reg1].fltVal - reg.reg[code.reg2].fltVal; break;
		case eCommand::mulf: reg.reg[9] = reg.reg[code.reg1].fltVal * reg.reg[code.reg2].fltVal; break;
		case eCommand::divf: 
			if (0 == reg.reg[code.reg2].fltVal)
				break;
			reg.reg[9] = reg.reg[code.reg1].fltVal / reg.reg[code.reg2].fltVal; break;
		}
		++reg.idx;
		break;

	case eCommand::negate:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		reg.reg[9] = !((bool)reg.reg[code.reg1]);
		++reg.idx;
		break;

	case eCommand::adds:
	{
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;
		const string s0 = variant2str(reg.reg[code.reg1]);
		const string s1 = variant2str(reg.reg[code.reg2]);
		reg.reg[9] = (s0 + s1).c_str();
		++reg.idx;
	}
	break;

	case eCommand::eqi:
	case eCommand::eqf:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (reg.reg[code.reg1].vt == VT_BOOL)
		{
			// force converting int
			reg.reg[code.reg1] = ((bool)reg.reg[code.reg1]) ? 1 : 0;
		}
		if (reg.reg[code.reg2].vt == VT_BOOL)
		{
			// force converting int
			reg.reg[code.reg2] = ((bool)reg.reg[code.reg2]) ? 1 : 0;
		}
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.reg[code.reg1] == reg.reg[code.reg2]);
		++reg.idx;
		break;

	case eCommand::eqs:
	{
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;

		const string str1 = common::variant2str(reg.reg[code.reg1]);
		const string str2 = common::variant2str(reg.reg[code.reg2]);
		reg.cmp = (str1 == str2);
		++reg.idx;
	}
	break;

	case eCommand::eqic:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg[code.reg1].vt == VT_BOOL)
		{
			// force converting integer
			reg.reg[code.reg1] = (int)((bool)reg.reg[code.reg1]) ? 1 : 0;
		}
		//if (varType != reg.reg[code.reg1].vt)
		//	goto $error_semantic;
		//if (varType != code.var1.vt)
		//	goto $error_semantic;

		// can compare int, float
		if ((VT_INT != reg.reg[code.reg1].vt) && (VT_R4 != reg.reg[code.reg1].vt))
			goto $error_semantic;
		if ((VT_INT != code.var1.vt) && (VT_R4 != code.var1.vt))
			goto $error_semantic;
		reg.cmp = (int)reg.reg[code.reg1] == (int)code.var1;
		++reg.idx;
		break;

	case eCommand::eqfc:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg[code.reg1].vt == VT_BOOL)
		{
			// force converting float
			reg.reg[code.reg1] = ((bool)reg.reg[code.reg1]) ? 1.f : 0.f;
		}
		// can compare int, float
		if ((VT_INT != reg.reg[code.reg1].vt) && (VT_R4 != reg.reg[code.reg1].vt))
			goto $error_semantic;
		if ((VT_INT != code.var1.vt) && (VT_R4 != code.var1.vt))
			goto $error_semantic;

		reg.cmp = (float)reg.reg[code.reg1] == (float)code.var1;
		++reg.idx;
		break;

	case eCommand::eqsc:
	{
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != code.var1.vt)
			goto $error_semantic;

		reg.cmp = reg.reg[code.reg1] == code.var1;
		++reg.idx;
	}
	break;


	case eCommand::lesi:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.reg[code.reg1].intVal < reg.reg[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::lesf:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.reg[code.reg1].fltVal < reg.reg[code.reg2].fltVal);
		++reg.idx;
		break;

	case eCommand::leqi:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.reg[code.reg1].intVal <= reg.reg[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::leqf:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.reg[code.reg1].fltVal <= reg.reg[code.reg2].fltVal);
		++reg.idx;
		break;

	case eCommand::gri:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.reg[code.reg1].intVal > reg.reg[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::grf:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.reg[code.reg1].fltVal > reg.reg[code.reg2].fltVal);
		++reg.idx;
		break;

	case eCommand::greqi:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.reg[code.reg1].intVal >= reg.reg[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::greqf:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.reg[code.reg1].fltVal >= reg.reg[code.reg2].fltVal);
		++reg.idx;
		break;

	case eCommand::opand:
	case eCommand::opor:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (reg.reg.size() <= code.reg2)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (varType != reg.reg[code.reg2].vt)
			goto $error_semantic;
		if (eCommand::opand == code.cmd)
			reg.cmp = ((bool)reg.reg[code.reg1] && (bool)reg.reg[code.reg2]);
		else
			reg.cmp = ((bool)reg.reg[code.reg1] || (bool)reg.reg[code.reg2]);
		reg.reg[9] = reg.cmp;
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

		const eCallbackState res = 
			m_callback->Function(*this, code.str1, funcName, m_callbackArgPtr);
		if (res == eCallbackState::Wait)
		{
			// wait until resume
			m_state = eState::WaitCallback;
		}
		else
		{
			++reg.idx;
		}
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
	dbg::Logc(3, "Error cVirtualMachine::Execute() not valid command. '%s' index=%d, cmd=%d\n"
		, m_code.m_fileName.c_str(), reg.idx, (int)code.cmd);
	WriteTraceLog(m_trace2);
	m_state = eState::Stop;
	return false;

$error_semantic:
	dbg::Logc(3, "Error cVirtualMachine::Execute() Semantic Error.' %s', index=%d, cmd=%d\n"
		, m_code.m_fileName.c_str(), reg.idx, (int)code.cmd);
	WriteTraceLog(m_trace2);
	m_state = eState::Stop;
	return false;

$error_memory:
	dbg::Logc(3, "Error cVirtualMachine::Execute() Memory Error. '%s', index=%d, type=%d, reg1=%d, reg2=%d\n"
		, m_code.m_fileName.c_str(), reg.idx, (int)code.cmd, code.reg1, code.reg2);
	WriteTraceLog(m_trace2);
	m_state = eState::Stop;
	return false;
}


// executed code index log
// ex) 0-10, 15-20, 101-101
void cVirtualMachine::CodeTraceLog(vector<ushort> &trace)
{
	if (trace.empty())
	{
		// start idx-idx
		trace.push_back(m_reg.idx);
		trace.push_back(m_reg.idx);
	}
	else
	{
		const int curIdx = trace.back();
		if (curIdx == m_reg.idx)
			return; // nothing~
		
		// contineous instruction index?
		if ((curIdx + 1) == m_reg.idx)
		{
			trace.back() = m_reg.idx;
		}
		else
		{
			// jump instruction index
			trace.push_back(m_reg.idx);
			trace.push_back(m_reg.idx);
		}
	}
}


// write tracelog for debugging
void cVirtualMachine::WriteTraceLog(const vector<ushort> &trace)
{
	if (trace.empty())
		return;

	StrPath fileName = m_code.m_fileName.GetFileNameExceptExt2();
	fileName += ".trace";
	std::ofstream ofs(fileName.c_str());
	if (!ofs.is_open())
		return;
	for (auto t : trace)
		ofs << t << std::endl;
}


// set executed code index log on/off
void cVirtualMachine::SetCodeTrace(const bool isCodeTrace) 
{
	m_isCodeTraceLog = isCodeTrace;
}


// clear executed code index log
// isTakeLast: m_trace.back() take, clear remains
void cVirtualMachine::ClearCodeTrace(
	const bool isTakeLast //=false
)
{
	const int back = (isTakeLast && !m_trace.empty()) ? m_trace.back() : -1;
	m_trace.clear();
	
	if (isTakeLast)
	{
		m_trace.push_back(back);
		m_trace.push_back(back);
	}
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
