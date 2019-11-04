
#include "stdafx.h"
#include "event.h"

using namespace common;
using namespace common::script;


cEvent::cEvent()
{
}

cEvent::cEvent(const StrId &name
	, const map<string, variant_t> &vars //= {}
)
	: m_name(name)
	, m_vars(vars)
{
}

cEvent::~cEvent()
{
	for (auto &kv : m_vars)
		common::clearvariant(kv.second);
	m_vars.clear();
}

