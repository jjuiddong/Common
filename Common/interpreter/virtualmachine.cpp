
#include "stdafx.h"
#include "virtualmachine.h"

using namespace common;
using namespace common::script;


cVirtualMachine::cVirtualMachine()
	: m_state(eState::Stop)
{
}

cVirtualMachine::~cVirtualMachine()
{
	Clear();
}


bool cVirtualMachine::Init(const cIntermediateCode &code)
{
	Clear();

	m_reg.idx = 0;
	m_reg.cmp = false;

	m_code = code;
	return true;
}


bool cVirtualMachine::Update(const float deltaSeconds)
{

	return true;
}


bool cVirtualMachine::Run()
{
	return true;
}


bool cVirtualMachine::Stop()
{
	return true;
}


void cVirtualMachine::Clear()
{
	Stop();
	m_symbTable.Clear();
	m_code.Clear();
}
