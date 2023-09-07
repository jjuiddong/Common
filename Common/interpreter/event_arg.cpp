
#include "stdafx.h"
#include "event_arg.h"

using namespace common;
using namespace common::script;


// execute event, update symboltable
bool cEventArg::ProcessVM(cVirtualMachine& vm)
{
	__super::ProcessVM(vm);

	for (auto& kv : m_symbTable.m_vars)
		for (auto& var : kv.second)
			vm.m_symbTable.m_vars[kv.first][var.first] = var.second;
	for (auto& kv : m_symbTable.m_varMap)
		vm.m_symbTable.m_varMap[kv.first] = kv.second;

	return true;
}
