
#include "stdafx.h"
#include "pin.h"

using namespace framework;
using namespace vprog;


cPin::cPin(int id, const char* name, PinType type)
	: m_id(id)
	, m_node(nullptr)
	, m_name(name)
	, m_type(type)
	, m_kind(PinKind::Input)
{
}

cPin::~cPin()
{
}
