
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
	case eCommand::addi:
	case eCommand::subi:
	case eCommand::muli:
	case eCommand::divi:
	case eCommand::eqi:
	case eCommand::lesi:
	case eCommand::eqic:
	case eCommand::leqi:
	case eCommand::gri:
	case eCommand::greqi:
	case eCommand::symboli:
	case eCommand::timer1:
	case eCommand::timer2:
		return VT_INT;

	case eCommand::ldfc:
	case eCommand::ldtim:
	case eCommand::getf:
	case eCommand::setf:
	case eCommand::addf:
	case eCommand::subf:
	case eCommand::mulf:
	case eCommand::divf:
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
	case eCommand::adds:
		return VT_BSTR;

	case eCommand::ldbc:
	case eCommand::ldcmp:
	case eCommand::ldncmp:
	case eCommand::getb:
	case eCommand::setb:
	case eCommand::symbolb:
		return VT_BOOL;

	case eCommand::call:
	case eCommand::jnz:
	case eCommand::jmp:
	case eCommand::nop:
	case eCommand::label:
	case eCommand::cmt:
	case eCommand::delay:
	default:
		break;
	}
	return VT_EMPTY;
}



//----------------------------------------------------
// sInstruction
//----------------------------------------------------
sInstruction::sInstruction(const sInstruction &rhs)
	: reg1(0), reg2(0)
{
	operator=(rhs);
}
sInstruction::~sInstruction()
{
	common::clearvariant(var1);
}
sInstruction& sInstruction::operator=(const sInstruction &rhs)
{
	if (this != &rhs)
	{
		cmd = rhs.cmd;
		str1 = rhs.str1;
		str2 = rhs.str2;
		reg1 = rhs.reg1;
		reg2 = rhs.reg2;
		common::clearvariant(var1);
		var1 = common::copyvariant(rhs.var1);
	}
	return *this;
}
