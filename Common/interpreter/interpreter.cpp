
#include "stdafx.h"
#include "interpreter.h"

using namespace common;
using namespace common::script;


cInterpreter::cInterpreter()
	: m_state(eState::Stop)
{
}

cInterpreter::~cInterpreter()
{
	Clear();
}


bool cInterpreter::Update(const float deltaSeconds)
{
	RETV(eState::Stop == m_state, true);



	return true;
}


// read intermediate code
bool cInterpreter::Init(const StrPath &fileName)
{
	m_fileName = fileName;
	if (!m_code.Read(fileName))
		return false;

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
	if (!vm->Init(m_code))
		return false;

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
