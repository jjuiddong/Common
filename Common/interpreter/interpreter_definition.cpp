
#include "stdafx.h"
#include "interpreter_definition.h"

using namespace common;
using namespace common::script;


cEvent::cEvent()
{
}

cEvent::cEvent(const StrId &name
	, const map<StrId, variant_t> &vars //= {}
)
	: m_name(name)
	, m_vars(vars)
{
}

cEvent::~cEvent()
{
}

