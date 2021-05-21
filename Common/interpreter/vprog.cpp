
#include "stdafx.h"
#include "vprog.h"

using namespace vprog;


// Convert ePinType to Value Type
VARTYPE vprog::GetPin2VarType(const ePinType::Enum type)
{
	VARTYPE vt = VT_EMPTY;
	switch (type)
	{
	case ePinType::Bool: return VT_BOOL;
	case ePinType::Enums:
	case ePinType::Any://NotDef:
	case ePinType::Int: return VT_INT;
	case ePinType::Float: return VT_R4;
	case ePinType::String: return VT_BSTR;
	case ePinType::Flow: return VT_EMPTY;
	case ePinType::Array: return VT_BYREF; // tricky type
	case ePinType::Map: return VT_BYREF; // tricky type
	default:
		assert(!"vprog::GetPin2VarType() Error symbol parse error!!");
		break;
	}
	return VT_EMPTY;
}


// Is Variable Type? int, float .. etc
// is primitive type?
bool vprog::IsVarType(const ePinType::Enum type)
{
	switch (type)
	{
	case ePinType::Bool:
	case ePinType::Enums:
	case ePinType::Int:
	case ePinType::Float:
	case ePinType::String:
		return true;
	default:
		return false;
	}
}

