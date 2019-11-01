
#include "stdafx.h"
#include "virtualmachine.h"

using namespace common;
using namespace common::script;


cVirtualMachine::cVirtualMachine()
	: m_state(eState::Stop)
	, m_callback(nullptr)
	, m_callbackArgPtr(nullptr)
{
}

cVirtualMachine::~cVirtualMachine()
{
	Clear();
}


bool cVirtualMachine::Init(const cIntermediateCode &cmd, iFunctionCallback *callback
	, void *arg //= nullptr
)
{
	Clear();

	m_reg.idx = 0;
	m_reg.cmp = false;

	m_code = cmd;
	m_callback = callback;
	m_callbackArgPtr = arg;
	return true;
}


bool cVirtualMachine::Update(const float deltaSeconds)
{
	RETV(eState::Stop == m_state, true);
	RETV(!m_code.IsLoaded(), true);

	Execute(m_reg);

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

	return true;
}


// execute intermediate code commandset
bool cVirtualMachine::Execute(sRegister &reg)
{
	if (m_code.m_codes.size() <= reg.idx)
		return false; // end of code

	sCommandSet &code = m_code.m_codes[reg.idx];

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
			reg.idx = it->second + 1;
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
		reg.idx = it->second + 1;
	}
	break;

	case eCommand::call:
	{
		if (!m_callback)
			goto $error;
		
		vector<string> out;
		common::tokenizer(code.str1, "::", "", out);
		if (out.empty())
			goto $error;

		m_callback->Function(m_symbTable, code.str1.c_str(), out[0], m_callbackArgPtr);
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
}
