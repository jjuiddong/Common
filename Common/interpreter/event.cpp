
#include "stdafx.h"
#include "event.h"

using namespace common;
using namespace common::script;


cEvent::cEvent()
	:  m_delayTime(0.f)
	, m_vmId(-1)
{
}

cEvent::cEvent(const StrId &name
	, const map<string, variant_t> &vars //= {}
	, const bool isUnique //= false
	, const int vmId //=-1
)
	: m_name(name)
	, m_vars(vars)
	, m_isUnique(isUnique)
	, m_delayTime(0.f)
	, m_vmId(vmId)
{
}

cEvent::~cEvent()
{
	for (auto &kv : m_vars)
		common::clearvariant(kv.second);
	m_vars.clear();
}

