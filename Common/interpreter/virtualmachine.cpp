
#include "stdafx.h"
#include "virtualmachine.h"

using namespace common;
using namespace common::script;

namespace
{
	const float TIME_SYNC_INSTRUCTION = 0.5f; // seconds unit
	const float TIME_SYNC_REGISTER = 5.0f; // seconds unit
	const float TIME_SYNC_SYMBOL = 3.0f; // seconds unit
}


cVirtualMachine::cVirtualMachine(const string &name)
	: m_id(common::GenerateId())
	, m_parentId(-1)
	, m_state(eState::Stop)
	, m_name(name)
	, m_isCodeTraceLog(false)
	, m_isDebugging(false)
	, m_events(128) // maximum queue size
	, m_itpr(nullptr)
{
	ClearNSync(true);
}

cVirtualMachine::~cVirtualMachine()
{
	Clear();
}


// initialize virtual machine
// parentVmId: parent virtual machine id, -1:root
// scopeName: execute command scope name
bool cVirtualMachine::Init(cInterpreter* interpreter, const cIntermediateCode &code
	, const int parentVmId //=-1
	, const string& scopeName //= ""
)
{
	Clear();

	m_parentId = parentVmId;
	m_scopeName = scopeName;
	m_itpr = interpreter;
	m_reg.idx = 0;
	m_reg.exeIdx = 0;
	m_reg.cmp = false;

	m_code = code;
	m_symbTable = code.m_variables; // initial variable

	EnableNetworkSync(true); // default network synchronize

	// update tick/timer
	for (auto &tick : m_code.m_timer1Events)
	{
		m_ticks.push_back(
			{ common::GenerateId()
			, tick.first // tick name
			, tick.second / 1000.f // tick interval (convert seconds unit)
			, (float)tick.second / 1000.f } // tick decrease time (convert seconds unit)
		);
	}

	m_stack.reserve(32);
	m_delayEvents.reserve(16);
	return true;
}


// add function execute module
bool cVirtualMachine::AddModule(iModule *mod)
{
	auto it = std::find(m_modules.begin(), m_modules.end(), mod);
	if (m_modules.end() != it)
		return false; // already exist
	m_modules.push_back(mod);
	return true;
}


// remove function execute module
bool cVirtualMachine::RemoveModule(iModule *mod)
{
	auto it = std::find(m_modules.begin(), m_modules.end(), mod);
	if (m_modules.end() == it)
		return false; // not exist
	(*it)->CloseModule(*this);
	m_modules.erase(it);
	return true;
}


// execute event, instruction
bool cVirtualMachine::Process(const float deltaSeconds)
{
	RETV((eState::Stop == m_state) || (eState::WaitCallback == m_state), true);
	RETV(!m_code.IsLoaded(), true);

	ProcessDelayEvent(deltaSeconds);
	ProcessEvent(deltaSeconds);
	ProcessTimer(deltaSeconds);

	ExecuteInstruction(deltaSeconds, m_reg);

	if (m_isCodeTraceLog)
		CodeTraceLog(m_trace);

	if (m_isDebugging)
		CodeTraceLog(m_trace2);

	// network synchronize?
	if (m_nsync.enable)
	{
		m_nsync.regTime += deltaSeconds;
		m_nsync.instTime += deltaSeconds;
		m_nsync.symbTime += deltaSeconds;

		// sync delay instruction (check next instruction is delay node?)
		// 'm_reg.idx' is next execute instruction code index
		// ldtim is previous delay command
		if (script::eCommand::ldtim == m_code.m_codes[m_reg.idx].cmd)
		{
			m_nsync.instTime = TIME_SYNC_INSTRUCTION + 1.f;
			m_nsync.regTime = TIME_SYNC_REGISTER + 1.f;
		}

		if (m_nsync.regTime > TIME_SYNC_REGISTER)
		{
			m_nsync.regTime = 0.f;
			m_nsync.regStreaming = true;
		}
		if (m_nsync.instTime > TIME_SYNC_INSTRUCTION)
		{
			m_nsync.instTime = 0.f;
			m_nsync.instStreaming = true;
		}
		if (m_nsync.symbTime > TIME_SYNC_SYMBOL)
		{
			m_nsync.symbTime = 0.f;
			m_nsync.symbStreaming = true;
		}
	}
	//~

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

	// wait -> resume process, trace instruction log
	if (m_isCodeTraceLog)
		CodeTraceLog(m_trace);
	if (m_isDebugging)
		CodeTraceLog(m_trace2);
	return true;
}


// stop virtual machine
bool cVirtualMachine::Stop()
{
	m_state = eState::Stop;
	m_reg.idx = UINT_MAX;
	m_reg.exeIdx = UINT_MAX;

	// clear event, timer, stack
	m_events.clear();
	m_delayEvents.clear();
	m_timers.clear();
	m_ticks.clear();
	m_syncs.clear();
	m_stack.clear();

	// close vm module
	for (auto &mod : m_modules)
		mod->CloseModule(*this);

	return true;
}


bool cVirtualMachine::PushEvent(const cEvent &evt)
{
	// check same event
	if (evt.m_flags & (int)eEventFlags::Unique)
	{
		uint cnt = 0;
		uint i = m_events.m_front;
		uint size = m_events.size();
		while (cnt++ < size)
		{
			const auto &e = m_events.m_buffer[i];
			if (e.m_name != evt.m_name)
				continue;

			bool isSame = true;
			for (auto &kv : evt.m_vars)
			{
				auto it = e.m_vars.find(kv.first);
				if (e.m_vars.end() == it) {
					isSame = false;
					break;
				}
				if (it->second != kv.second)
				{
					isSame = false;
					break;
				}
			}
			if (isSame)
				return false; // already exist same event

			i = (i + 1) % m_events.SIZE;
		}
	}
	if (evt.m_delayTime > 0.f)
		m_delayEvents.push_back(evt);
	else
		m_events.push(evt);
	return true;
}


// set timer
// name: timer name, format: name_<number>
// timeMillis: timeout, milliseconds 
// isLoop: infinity loop?
// syncId: sync timeout?, syncId
// return: timer id, -1: fail
int cVirtualMachine::SetTimer(const string& name, const int timeMillis
	, const bool isLoop //= false
	, const int syncId //=-1
)
{
	int timerId = 0;
	auto it = std::find_if(m_timers.begin(), m_timers.end()
		, [&](const auto& a) { return a.name == name; });
	if (m_timers.end() != it)
	{
		it->t = it->interval; // initialize
		timerId = it->id;
	}
	else
	{
		timerId = common::GenerateId();
		m_timers.push_back(
			{ timerId
			, name
			, timeMillis / 1000.f // timer interval (convert seconds unit)
			, (float)timeMillis / 1000.f  // timer decrease time (convert seconds unit)
			, isLoop // continuous call timer event?
			, syncId
			}
		);
	}
	return timerId;
}


// stop timer
bool cVirtualMachine::StopTimer(const int timerId)
{
	auto it = std::find_if(m_timers.begin(), m_timers.end()
		, [&](const auto &a) { return a.id == timerId; });
	if (m_timers.end() == it)
		return false;
	m_timers.erase(it);
	return true;
}


// stop tick
bool cVirtualMachine::StopTick(const int tickId)
{
	auto it = std::find_if(m_ticks.begin(), m_ticks.end()
		, [&](const auto &a) { return a.id == tickId; });
	if (m_ticks.end() == it)
		return false;
	m_ticks.erase(it);
	return true;
}


// process event
// execute event only waiting state
// waiting state is nop instruction state
bool cVirtualMachine::ProcessEvent(const float deltaSeconds)
{
	RETV(eState::Wait != m_state, false);
	RETV(m_events.empty(), false);

	cEvent &evt = m_events.front();
	if (evt.m_name == "@@symbol@@")
	{
		// update symbol value
		for (auto& kv : evt.m_vars)
		{
			vector<string> out;
			common::tokenizer(kv.first.c_str(), "::", "", out);
			if (out.size() >= 2)
				m_symbTable.Set(out[0].c_str(), out[1].c_str(), kv.second);
		}
	}
	else
	{
		// event trigger
		const uint addr = m_code.FindJumpAddress(evt.m_name);
		const bool isSyncFlow = (evt.m_flags & (int)eEventFlags::SyncFlow) > 0;

		// update symboltable
		// tricky code: ignore global event if no handler
		//              no ignore custom event
		const bool isUpdateSymbol = (UINT_MAX != addr)
			|| (evt.m_name.find("_") != nullptr);
		if (isUpdateSymbol)
		{
			for (auto &kv : evt.m_vars)
			{
				vector<string> out;
				common::tokenizer(kv.first.c_str(), "::", "", out);
				if (out.size() >= 2)
					m_symbTable.Set(out[0].c_str(), out[1].c_str(), kv.second);
			}
			for (auto &kv : evt.m_vars2)
			{
				vector<string> out;
				common::tokenizer(kv.first.c_str(), "::", "", out);
				if (out.size() >= 2)
					m_symbTable.Set(out[0].c_str(), out[1].c_str(), kv.second);
			}
			for (auto& kv : evt.m_vars3)
			{
				vector<string> out;
				common::tokenizer(kv.first.c_str(), "::", "", out);
				if (out.size() >= 2)
					m_symbTable.Set(out[0].c_str(), out[1].c_str(), kv.second);
			}
		}

		if (UINT_MAX != addr)
		{
			m_reg.idx = addr; // jump instruction code
			m_state = eState::Run;
		}
		else
		{
			// no event handler
		}
	}

	m_events.pop();
	return true;
}


// process delay event
bool cVirtualMachine::ProcessDelayEvent(const float deltaSeconds)
{
	RETV(eState::Wait != m_state, false);
	RETV(m_delayEvents.empty(), false);

	for (auto& evt : m_delayEvents)
		evt.m_delayTime -= deltaSeconds;

	for (uint i=0; i < m_delayEvents.size(); ++i)
	{
		auto& evt = m_delayEvents[i];
		if (evt.m_delayTime < 0.f)
		{
			const uint addr = m_code.FindJumpAddress(evt.m_name);
			if (UINT_MAX != addr)
			{
				// update symboltable
				for (auto& kv : evt.m_vars)
				{
					vector<string> out;
					common::tokenizer(kv.first.c_str(), "::", "", out);
					if (out.size() >= 2)
						m_symbTable.Set(out[0].c_str(), out[1].c_str(), kv.second);
				}
				for (auto& kv : evt.m_vars2)
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

			common::removevector2(m_delayEvents, i);
			return true;
		}
	}
	return false;
}


// process tick / timer
// check tick interval time, call tick event
// check timer delay time, call timer event
// execute event only waiting state
bool cVirtualMachine::ProcessTimer(const float deltaSeconds)
{
	RETV(eState::Wait != m_state, false);

	if (!m_ticks.empty())
	{
		for (auto &tick : m_ticks)
		{
			tick.t -= deltaSeconds;
			if (tick.t < 0.f)
			{
				// tick event trigger
				// tick id output
				const string scopeName = (tick.name + "::id").c_str();
				PushEvent(cEvent(tick.name, { {scopeName, tick.id} }));
				tick.t = tick.interval;
				//break;
			}
		}
	}

	if (!m_timers.empty())
	{
		auto it = m_timers.begin();
		while (it != m_timers.end())
		{
			sTimer &timer = *it;
			timer.t -= deltaSeconds;
			if (timer.t < 0.f)
			{
				// timer event trigger
				// timer id output
				const string timerName = string(timer.name.c_str()) + "_event";
				PushEvent(cEvent(timerName));

				// if sync object, disable
				if (timer.syncId >= 0)
				{
					auto it2 = std::find_if(m_syncs.begin(), m_syncs.end()
						, [&](const auto& a) { return a.id == timer.syncId; });
					if (m_syncs.end() != it2)
						it2->enable = false; // timeout, sync done
				}
				//~

				if (timer.isLoop)
				{
					timer.t = timer.interval;
					++it; // no remove
				}
				else
				{
					it = m_timers.erase(it); // remove this timer
				}
			}
			else
			{
				++it;
			}
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

	case eCommand::geti:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (m_symbTable.Get(code.str1, code.str2, reg.reg[code.reg1]))
		{
			if (varType == reg.reg[code.reg1].vt)
			{
				++reg.idx;
			}
			else if (IsAssignable(reg.reg[code.reg1].vt, varType))
			{
				reg.reg[code.reg1] = (int)reg.reg[code.reg1];
				++reg.idx;
			}
			else
			{
				goto $error_semantic;
			}
		}
		else
		{
			goto $error;
		}
		break;

	case eCommand::getf:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (m_symbTable.Get(code.str1, code.str2, reg.reg[code.reg1]))
		{
			if (varType == reg.reg[code.reg1].vt)
			{
				++reg.idx;
			}
			else if (IsAssignable(reg.reg[code.reg1].vt, varType))
			{
				reg.reg[code.reg1] = (float)reg.reg[code.reg1];
				++reg.idx;
			}
			else
			{
				goto $error_semantic;
			}
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
	case eCommand::sets:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (varType != reg.reg[code.reg1].vt)
			goto $error_semantic;
		if (m_symbTable.Set(code.str1, code.str2, reg.reg[code.reg1]))
			++reg.idx;
		else
			goto $error;
		break;

	case eCommand::seti:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (varType == reg.reg[code.reg1].vt)
		{
			if (!m_symbTable.Set(code.str1, code.str2, reg.reg[code.reg1]))
				goto $error;
		}
		else if (IsAssignable(reg.reg[code.reg1].vt, varType))
		{
			if (!m_symbTable.Set(code.str1, code.str2, (int)reg.reg[code.reg1]))
				goto $error;
		}
		else
		{
			goto $error_semantic;
		}
		++reg.idx;
		break;

	case eCommand::setf:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (varType == reg.reg[code.reg1].vt)
		{
			if (!m_symbTable.Set(code.str1, code.str2, reg.reg[code.reg1]))
				goto $error;
		}
		else if (IsAssignable(reg.reg[code.reg1].vt, varType))
		{
			if (!m_symbTable.Set(code.str1, code.str2, (float)reg.reg[code.reg1]))
				goto $error;
		}
		else
		{
			goto $error_semantic;
		}
		++reg.idx;
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
	case eCommand::remi:
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
		case eCommand::divi: 
			if (0 == reg.reg[code.reg2].intVal)
				break;
			reg.reg[9] = reg.reg[code.reg1].intVal / reg.reg[code.reg2].intVal; 
			break;
		case eCommand::remi: 
			if (0 == reg.reg[code.reg2].intVal)
				break;
			reg.reg[9] = reg.reg[code.reg1].intVal % reg.reg[code.reg2].intVal; 
			break;
		}		
		++reg.idx;
		break;

	case eCommand::addf:
	case eCommand::subf:
	case eCommand::mulf:
	case eCommand::divf:
	case eCommand::remf:
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
			reg.reg[9] = reg.reg[code.reg1].fltVal / reg.reg[code.reg2].fltVal; 
			break;
		case eCommand::remf:
			if (0.f == reg.reg[code.reg2].fltVal)
				break;
			reg.reg[9] = (float)((int)reg.reg[code.reg1].fltVal % (int)reg.reg[code.reg2].fltVal); 
			break;
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
		string funcName;
		int nodeId;
		std::tie(funcName, nodeId) = cSymbolTable::ParseScopeName(code.str1);
		if (funcName.empty())
			goto $error;
		
		bool isFind = false;
		for (auto &mod : m_modules)
		{
			const eModuleResult res = mod->Execute(*this, code.str1, funcName);
			if ((eModuleResult::None == res) || (eModuleResult::NoHandler == res))
				continue;
			isFind = true;
			if (eModuleResult::Wait == res)
				m_state = eState::WaitCallback; // wait until resume
			else
				++reg.idx;
			break;
		}
		if (!isFind)
			goto $error_call;
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

	case eCommand::synct:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (!IsAssignable(varType, reg.reg[code.reg1].vt))
			goto $error_semantic;
		if (varType != code.var1.vt)
			goto $error_semantic;

		InitSyncTimer((int)code.var1, (int)reg.reg[code.reg1]);
		++reg.idx;
		break;

	case eCommand::synci:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (!IsAssignable(varType, reg.reg[code.reg1].vt))
			goto $error_semantic;
		if (varType != code.var1.vt)
			goto $error_semantic;

		InitSyncFlow((int)reg.reg[code.reg1], (int)code.var1);
		++reg.idx;
		break;

	case eCommand::synco:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (!IsAssignable(varType, reg.reg[code.reg1].vt))
			goto $error_semantic;
		if (varType != code.var1.vt)
			goto $error_semantic;

		InitSyncOrderFlow((int)reg.reg[code.reg1], (int)code.var1);
		++reg.idx;
		break;

	case eCommand::sync:
		if (reg.reg.size() <= code.reg1)
			goto $error_memory;
		if (!IsAssignable(varType, reg.reg[code.reg1].vt))
			goto $error_semantic;
		if (varType != code.var1.vt)
			goto $error_semantic;

		reg.cmp = CheckSync((int)reg.reg[code.reg1], (int)code.var1);
		++reg.idx;
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

$error_call:
	dbg::Logc(3, "Error cVirtualMachine::Execute() Calling Function Error. '%s', index=%d, type=%d, reg1=%d, reg2=%d\n"
		, m_code.m_fileName.c_str(), reg.idx, (int)code.cmd, code.reg1, code.reg2);
	WriteTraceLog(m_trace2);
	// vm working, no stop
	// todo: send error message
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


// is assignable?
// srcVarType: source variable type
// dstVarType: destination variable type
// dstVarType = srcVarType
bool cVirtualMachine::IsAssignable(const VARTYPE srcVarType, const VARTYPE dstVarType)
{
	if (srcVarType == dstVarType)
		return true;
	switch (srcVarType)
	{
	case VT_INT:
		switch (dstVarType)
		{
		case VT_R4:
			return true;
		}
		break;

	case VT_R4: return IsAssignable(dstVarType, srcVarType);
	}
	return false;
}


// initialize sync time out
// timeOut: millisecond unit
bool cVirtualMachine::InitSyncTimer(const int syncId, const int timeOut)
{
	if (timeOut <= 0)
		return true; // nothing to do

	auto it = std::find_if(m_syncs.begin(), m_syncs.end()
		, [&](auto& a) { return a.id == syncId; });
	if (m_syncs.end() == it)
	{
		// add new sync object
		sSync sync;
		sync.id = syncId;
		sync.type = 0; // Sync type
		sync.enable = true;
		sync.syncs.clear();
		sync.timerId = SetTimer(common::format("Sync_%d-TimeOut", syncId), timeOut, false, syncId);

		if (m_itpr && m_itpr->m_listener)
			m_itpr->m_listener->SetTimeOutResponse(m_id, common::format("Sync_%d", syncId)
				, sync.timerId, timeOut);

		m_syncs.push_back(sync);
	}
	else
	{
		// already exist?, intialize sync object
		sSync &sync = *it;
		sync.id = syncId;
		sync.enable = true;
		sync.syncs.clear();
		StopTimer(sync.timerId);

		if (m_itpr && m_itpr->m_listener)
			m_itpr->m_listener->ClearTimeOutResponse(m_id, sync.timerId, 0);

		sync.timerId = SetTimer(common::format("Sync_%d-TimeOut", syncId), timeOut, false, syncId);

		if (m_itpr && m_itpr->m_listener)
			m_itpr->m_listener->SetTimeOutResponse(m_id, common::format("Sync_%d", syncId)
				, sync.timerId, timeOut);
	}

	return true;
}


// initialize sync flow pin id
// pinCount: synchronize pin count
bool cVirtualMachine::InitSyncFlow(const int syncId, const int pinCount)
{
	auto it = std::find_if(m_syncs.begin(), m_syncs.end()
		, [&](auto& a) { return a.id == syncId; });
	if (m_syncs.end() == it)
	{
		sSync sync;
		sync.id = syncId;
		sync.type = 0; // Sync type
		sync.enable = true;
		sync.timerId = -1;
		sync.syncs.clear();
		for (int i=0; i < pinCount; ++i)
			sync.syncs.push_back({ i, false });
		m_syncs.push_back(sync);
	}
	else
	{
		it->enable = true; // clear flag
		it->syncs.clear();
		for (int i = 0; i < pinCount; ++i)
			it->syncs.push_back({ i, false });
	}
	return true;
}


// initialize SyncOrder
bool cVirtualMachine::InitSyncOrderFlow(const int syncId, const int pinCount)
{
	auto it = std::find_if(m_syncs.begin(), m_syncs.end()
		, [&](auto& a) { return a.id == syncId; });
	if (m_syncs.end() == it)
	{
		sSync sync;
		sync.id = syncId;
		sync.type = 1; // SyncOrder type
		sync.enable = true;
		sync.timerId = -1;
		sync.syncs.clear();
		for (int i = 0; i < pinCount; ++i)
			sync.syncs.push_back({ i, false });
		m_syncs.push_back(sync);
	}
	else
	{
		it->enable = true; // clear flag
		it->syncs.clear();
		for (int i = 0; i < pinCount; ++i)
			it->syncs.push_back({ i, false });
	}
	return true;
}


// check sync
// pinIdx: synchronize pin idx
bool cVirtualMachine::CheckSync(const int syncId, const int pinIdx)
{
	auto it = std::find_if(m_syncs.begin(), m_syncs.end()
		, [&](auto& a) { return a.id == syncId; });
	if (m_syncs.end() == it)
		return false; // error return

	sSync& sync = *it;
	if (!sync.enable)
		return false; // all done, nothing to do

	if (0 == sync.type)
	{
		// Sync
		auto it2 = std::find_if(sync.syncs.begin(), sync.syncs.end()
			, [&](const auto& a) {return a.first == pinIdx; });
		if (sync.syncs.end() == it2)
			return false; // error return

		it2->second = true; // sync!

		// check all sync
		bool isAllSync = true;
		for (auto& s : sync.syncs)
		{
			if (!s.second)
			{
				isAllSync = false;
				break;
			}
		}
		if (!isAllSync)
			return false; // nothing to do

		// all sync?
		sync.enable = false; // sync done
		if (sync.timerId >= 0)
		{
			StopTimer(sync.timerId);

			if (m_itpr && m_itpr->m_listener)
				m_itpr->m_listener->ClearTimeOutResponse(m_id, sync.timerId, 0);
		}

		return true;
	}
	else if (1 == sync.type) 
	{
		// SyncOrder
		if (sync.syncs.size() <= (uint)pinIdx)
			return false; // error return

		bool isAlreadySync = false;
		for (uint i = 0; i < sync.syncs.size(); ++i)
		{
			if (pinIdx == i) continue;
			if (sync.syncs[i].second)
			{
				isAlreadySync = true;
				break;
			}
		}
		if (isAlreadySync)
			return false; // last enter
		
		sync.syncs[pinIdx].second = true;
		return true; // first enter
	}

	return false;
}


// enable network synchronize?
bool cVirtualMachine::EnableNetworkSync(const bool enable)
{
	m_nsync.enable = enable;
	m_nsync.sync = 0x00;
	m_nsync.regStreaming = enable;
	m_nsync.instStreaming = enable;
	m_nsync.symbStreaming = enable;
	m_nsync.regTime = TIME_SYNC_REGISTER + 1.0f;
	m_nsync.instTime = TIME_SYNC_INSTRUCTION + 1.0f;
	m_nsync.symbTime = TIME_SYNC_SYMBOL + 1.0f;
	return true;
}


// clear network synchronize data
// enable: enable network synchronize?
void cVirtualMachine::ClearNSync(
	const bool enable //=false
)
{
	m_nsync.enable = enable;
	m_nsync.sync = 0x00;
	m_nsync.regStreaming = false;
	m_nsync.instStreaming = false;
	m_nsync.symbStreaming = false;
	m_nsync.regTime = 0.f;
	m_nsync.instTime = 0.f;
	m_nsync.symbTime = 0.f;
	m_nsync.chSymbols.clear();
}


// enable executed code index log on/off
void cVirtualMachine::SetCodeTrace(const bool isCodeTrace) 
{
	m_isCodeTraceLog = isCodeTrace;
}


// initialize syncOrder
bool cVirtualMachine::ClearSyncOrder(const int syncId)
{
	auto it = std::find_if(m_syncs.begin(), m_syncs.end()
		, [&](auto& a) { return a.id == syncId; });
	if (m_syncs.end() == it)
		return false;
	it->enable = true; // clear flag
	for (auto& sync : it->syncs)
		sync.second = false;
	return true;
}


// clear executed code index log
// isTakeLast: m_trace.back() take, clear remains
// size: remove size, -1:all remove
void cVirtualMachine::ClearCodeTrace(
	const bool isTakeLast //=false
	, const int removeSize //=-1
)
{
	if (0 == removeSize)
		return; // nothing to do

	if (removeSize > 0)
	{
		const int rmSize = ((removeSize % 2) == 0) ? removeSize : removeSize + 1;
		if ((rmSize + 2) > (int)m_trace.size())
		{
			ClearCodeTrace(isTakeLast, -1);
		}
		else
		{
			int cnt = rmSize;
			while (cnt-- > 0)
				m_trace.erase(m_trace.begin());
		}
	}
	else
	{
		const int back = (isTakeLast && !m_trace.empty()) ? m_trace.back() : -1;
		m_trace.clear();

		if (isTakeLast)
		{
			m_trace.push_back(back);
			m_trace.push_back(back);
		}
	}
}


void cVirtualMachine::Clear()
{
	Stop();
	ClearNSync(true);
	m_parentId = -1;
	m_scopeName.clear();
	m_itpr = nullptr;
	m_symbTable.Clear();
	m_code.Clear();
	m_events.clear();
	m_timers.clear();
	m_ticks.clear();
	m_syncs.clear();
	m_stack.clear();
	for (auto &mod : m_modules)
		mod->CloseModule(*this);
	m_modules.clear();
}
