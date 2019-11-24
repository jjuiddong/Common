
#include "stdafx.h"
#include "vprog.h"

using namespace vprog;


// convert ePinType to Value Type
VARTYPE vprog::GetPin2VarType(const ePinType::Enum type)
{
	VARTYPE vt = VT_EMPTY;
	switch (type)
	{
	case ePinType::Bool: return VT_BOOL;
	case ePinType::Enums:
	case ePinType::Int: return VT_INT;
	case ePinType::Float: return VT_R4;
	case ePinType::String: return VT_BSTR;
	default:
		assert(!"vprog::GetPin2VarType() Error symbol parse error!!");
		break;
	}
	return VT_EMPTY;
}
