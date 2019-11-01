
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
bool cInterpreter::Init(const StrPath &fileName, iFunctionCallback *callback
	, void *arg //= nullptr
)
{
	m_fileName = fileName;
	if (!m_code.Read(fileName))
		return false;

	m_callback = callback;
	m_callbackArgPtr = arg;
	return true;
}


bool cInterpreter::Update(const float deltaSeconds)
{
	RETV(eState::Stop == m_state, true);

	for (auto &vm : m_vms)
		vm->Update(deltaSeconds);

	return true;
}


bool cInterpreter::Run()
{
	if (!m_code.IsLoaded())
		return false;

	cVirtualMachine *vm = nullptr;
	if (m_vms.empty())
	{
		vm = new cVirtualMachine();
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
