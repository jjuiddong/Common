
#include "stdafx.h"
#include "definition.h"

using namespace common;
using namespace common::script;


// return VarType corresspond eCommand
VARTYPE script::GetVarType(const eCommand::Enum cmd)
{
	switch (cmd)
	{
	case eCommand::ldic:
	case eCommand::geti:
	case eCommand::seti:
	case eCommand::eqi:
	case eCommand::lesi:
	case eCommand::eqic:
	case eCommand::leqi:
	case eCommand::gri:
	case eCommand::greqi:
	case eCommand::symboli:
		return VT_INT;

	case eCommand::ldfc:
	case eCommand::getf:
	case eCommand::setf:
	case eCommand::eqf:
	case eCommand::eqfc:
	case eCommand::lesf:
	case eCommand::leqf:
	case eCommand::grf:
	case eCommand::greqf:
	case eCommand::symbolf:
		return VT_R4;

	case eCommand::ldsc:
	case eCommand::sets:
	case eCommand::gets:
	case eCommand::eqs:
	case eCommand::eqsc:
	case eCommand::symbols:
		return VT_BSTR;

	case eCommand::ldbc:
	case eCommand::getb:
	case eCommand::setb:
	case eCommand::symbolb:
		return VT_BOOL;

	case eCommand::call:
	case eCommand::jnz:
	case eCommand::jmp:
	case eCommand::nop:
	case eCommand::label:
	default:
		break;
	}
	return VT_VOID;
}
