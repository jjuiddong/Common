
#include "stdafx.h"
#include "interpreter.h"

using namespace common;
using namespace common::script;


cInterpreter::cInterpreter()
	: m_state(eState::Stop)
	, m_callback(nullptr)
	, m_callbackArgPtr(nullptr)
{
}

cInterpreter::~cInterpreter()
{
	Clear();
}


// read intermediate code
// fileName : intermediatecode file name
bool cInterpreter::Init(const StrPath &icodeFileName, iFunctionCallback *callback
	, void *arg //= nullptr
)
{
	m_fileName = icodeFileName;
	if (!m_code.Read(icodeFileName))
		return false;

	m_callback = callback;
	m_callbackArgPtr = arg;
	return true;
}


// process interpreter virtualmachine
bool cInterpreter::Process(const float deltaSeconds)
{
	RETV(eState::Stop == m_state, true);
	RETV(eState::Debug == m_state, true);

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


// process one instruction for debugging
// only work in debug state
bool cInterpreter::ProcessOneStep(const float deltaSeconds)
{
	RETV(eState::Debug != m_state, true);

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


// run interpreter
bool cInterpreter::Run()
{
	if (!m_code.IsLoaded())
		return false;

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

	m_state = eState::Run;
	if (!vm->Init(m_code, m_callback, m_callbackArgPtr))
		return false;

	vm->Run();

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


// break run, change debugging state
bool cInterpreter::DebugBreak()
{
	RETV(eState::Stop == m_state, false);
	m_state = eState::Debug;
	return true;
}


// start with debugging state
bool cInterpreter::DebugRun()
{
	RETV(eState::Stop != m_state, false);
	
	if (!Run())
		return false;

	m_state = eState::Debug;
	return true;
}


// cancel debugging, change stop state
bool cInterpreter::DebugCancel()
{
	RETV(eState::Debug != m_state, false);
	Stop();
	return true;
}


// cancel debugging, change run state
bool cInterpreter::DebugToRun()
{
	RETV(eState::Debug != m_state, false);
	if (eState::Debug == m_state)
		m_state = eState::Run;
	return true;
}


bool cInterpreter::PushEvent(const cEvent &evt)
{
	m_events.push(evt);
	return true;
}


bool cInterpreter::IsRun() const { return eState::Run == m_state; }
bool cInterpreter::IsStop() const { return eState::Stop == m_state; }
bool cInterpreter::IsDebug() const { return eState::Debug == m_state; }


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
