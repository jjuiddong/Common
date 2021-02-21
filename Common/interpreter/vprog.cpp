
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
	case ePinType::NotDef:
	case ePinType::Int: return VT_INT;
	case ePinType::Float: return VT_R4;
	case ePinType::String: return VT_BSTR;
	case ePinType::Flow: return VT_EMPTY;
	default:
		assert(!"vprog::GetPin2VarType() Error symbol parse error!!");
		break;
	}
	return VT_EMPTY;
}


// Is Variable Type? int, float .. etc
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


// is variable type?
bool vprog::IsVariable(const eSymbolType::Enum type)
{
	switch (type)
	{
	case eSymbolType::Bool:
	case eSymbolType::Int:
	case eSymbolType::Float:
	case eSymbolType::String:
	case eSymbolType::Array:
	case eSymbolType::Map:
		return true;
	case eSymbolType::None:
	case eSymbolType::Enums:
	default:
		return false;
	}
}

