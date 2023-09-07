
#include "stdafx.h"
#include "event.h"

using namespace common;
using namespace common::script;


cEvent::cEvent()
	:  m_delayTime(0.f)
	, m_vmId(-1)
	, m_flags(0)
{
}

cEvent::cEvent(const StrId &name
	, const map<string, variant_t> &vars //= {}
	, const int flags // = 0
)
	: m_name(name)
	, m_vars(vars)
	, m_delayTime(0.f)
	, m_flags(flags)
{
}

cEvent::~cEvent()
{
	for (auto &kv : m_vars)
		common::clearvariant(kv.second);
	m_vars.clear();
}


// execute event, update symboltable
bool cEvent::ProcessVM(cVirtualMachine &vm)
{
	for (auto& kv : m_vars)
	{
		vector<string> out;
		common::tokenizer(kv.first.c_str(), "::", "", out);
		if (out.size() >= 2)
			vm.m_symbTable.Set(out[0].c_str(), out[1].c_str(), kv.second);
	}
	for (auto& kv : m_vars2)
	{
		vector<string> out;
		common::tokenizer(kv.first.c_str(), "::", "", out);
		if (out.size() >= 2)
			vm.m_symbTable.Set(out[0].c_str(), out[1].c_str(), kv.second);
	}
	for (auto& kv : m_vars3)
	{
		vector<string> out;
		common::tokenizer(kv.first.c_str(), "::", "", out);
		if (out.size() >= 2)
			vm.m_symbTable.Set(out[0].c_str(), out[1].c_str(), kv.second);
	}

	return true;
}
