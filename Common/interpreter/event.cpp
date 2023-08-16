
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

