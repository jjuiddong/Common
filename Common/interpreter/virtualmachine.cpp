
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


bool cVirtualMachine::Init(const cIntermediateCode &code, iFunctionCallback *callback
	, void *arg //= nullptr
)
{
	Clear();

	m_reg.idx = 0;
	m_reg.cmp = false;

	m_code = code;
	m_symbTable = code.m_variables; // initial variable
	m_callback = callback;
	m_callbackArgPtr = arg;

	return true;
}


bool cVirtualMachine::Process(const float deltaSeconds)
{
	RETV(eState::Stop == m_state, true);
	RETV(!m_code.IsLoaded(), true);

	ProcessEvent();
	ExecuteInstruction(m_reg);

	return true;
}


bool cVirtualMachine::Run()
{
	RETV(eState::Run == m_state, false); // already run

	m_state = eState::Run;

	// jump to entry point
	auto it = m_code.m_jmpMap.find("main");
	if (m_code.m_jmpMap.end() != it)
		m_reg.idx = it->second + 1;

	m_reg.cmp = false;

	return true;
}


bool cVirtualMachine::Stop()
{
	m_state = eState::Stop;
	m_reg.idx = UINT_MAX;
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
bool cVirtualMachine::ProcessEvent()
{
	RETV(eState::Wait != m_state, false);
	RETV(m_events.empty(), true);

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
		m_reg.idx = addr + 1; // jump instruction code
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


// execute intermediate code instruction
bool cVirtualMachine::ExecuteInstruction(sRegister &reg)
{
	if (m_code.m_codes.size() <= reg.idx)
		return false; // end of code

	sInstruction &code = m_code.m_codes[reg.idx];

	switch (code.cmd)
	{
	case eCommand::ldbc:
	case eCommand::ldic:
	case eCommand::ldfc:
	case eCommand::ldsc:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (GetVarType(code.cmd) != code.var1.vt)
			goto $error_semantic;
		reg.val[code.reg1] = code.var1;
		++reg.idx;
		break;

	case eCommand::ldcmp:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		reg.val[code.reg1] = (reg.cmp) ? variant_t((bool)true) : variant_t((bool)false);
		++reg.idx;
		break;

	case eCommand::getb:
	case eCommand::geti:
	case eCommand::getf:
	case eCommand::gets:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (m_symbTable.Get(code.str1, code.str2, reg.val[code.reg1]))
		{
			if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
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
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
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

	case eCommand::eqi:
	case eCommand::eqf:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (reg.val[code.reg1].vt == VT_BOOL)
		{
			reg.val[code.reg1].vt = VT_INT; // force converting int
			reg.val[code.reg1].intVal = (int)reg.val[code.reg1].boolVal;
		}
		if (reg.val[code.reg2].vt == VT_BOOL)
		{
			reg.val[code.reg2].vt = VT_INT; // force converting int
			reg.val[code.reg2].intVal = (int)reg.val[code.reg2].boolVal;
		}
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != reg.val[code.reg2].vt)
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
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = string((LPCTSTR)reg.val[code.reg1].bstrVal) 
			== string((LPCTSTR)reg.val[code.reg2].bstrVal);
		++reg.idx;
	}
	break;

	case eCommand::eqic:
	case eCommand::eqfc:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (reg.val[code.reg1].vt == VT_BOOL)
		{
			reg.val[code.reg1].vt = VT_INT; // force converting
			reg.val[code.reg1].intVal = (int)reg.val[code.reg1].boolVal;
		}
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != code.var1.vt)
			goto $error_semantic;

		reg.cmp = reg.val[code.reg1] == code.var1;
		++reg.idx;
		break;

	case eCommand::eqsc:
	{
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != code.var1.vt)
			goto $error_semantic;

		reg.cmp = string((LPCTSTR)reg.val[code.reg1].bstrVal)
			== string((LPCTSTR)code.var1.bstrVal);
		++reg.idx;
	}
	break;


	case eCommand::lesi:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].intVal < reg.val[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::lesf:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].fltVal < reg.val[code.reg2].fltVal);
		++reg.idx;
		break;

	case eCommand::leqi:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].intVal <= reg.val[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::leqf:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].fltVal <= reg.val[code.reg2].fltVal);
		++reg.idx;
		break;

	case eCommand::gri:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].intVal > reg.val[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::grf:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].fltVal > reg.val[code.reg2].fltVal);
		++reg.idx;
		break;

	case eCommand::greqi:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != reg.val[code.reg2].vt)
			goto $error_semantic;

		reg.cmp = (reg.val[code.reg1].intVal >= reg.val[code.reg2].intVal);
		++reg.idx;
		break;

	case eCommand::greqf:
		if (ARRAYSIZE(reg.val) <= code.reg1)
			goto $error_memory;
		if (ARRAYSIZE(reg.val) <= code.reg2)
			goto $error_memory;
		if (GetVarType(code.cmd) != reg.val[code.reg1].vt)
			goto $error_semantic;
		if (GetVarType(code.cmd) != reg.val[code.reg2].vt)
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
			reg.idx = it->second + 1; // jump instruction code
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
		reg.idx = it->second + 1; // jump instruction code
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

	case eCommand::label:
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
}
