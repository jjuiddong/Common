
#include "stdafx.h"
#include "vpl.h"

using namespace vpl;

static boost::bimap<ePinType, string> g_ePinTypeToStr =
	common::script::make_bimap<ePinType, string>(
		{
			{ePinType::Flow,"flow"},
			{ePinType::Bool,"bool"},
			{ePinType::Int,"int"},
			{ePinType::Float,"float"},
			{ePinType::Enums,"enums"},
			{ePinType::String,"string"},
			{ePinType::Object,"object"},
			{ePinType::Function,"function"},
			{ePinType::Delegate,"delegate"},
			{ePinType::Any,"any"},
			{ePinType::None,"none"},
			{ePinType::Array,"array"},
			{ePinType::Map,"map"},
			{ePinType::Blank,"blank"},
		}
	);

static boost::bimap<ePinKind, string> g_ePinKindToStr =
	common::script::make_bimap<ePinKind, string>(
		{
			{ePinKind::Output,"output"},
			{ePinKind::Input,"input"},
		}
	);

static boost::bimap<eNodeType, string> g_eNodeTypeToStr =
	common::script::make_bimap<eNodeType, string>(
		{
			{eNodeType::None,"none"},
			{eNodeType::Event,"event"},
			{eNodeType::Function,"function"},
			{eNodeType::Control,"control"},
			{eNodeType::Operator,"operator"},
			{eNodeType::Macro,"macro"},
			{eNodeType::Variable,"variable"},
			{eNodeType::Comment,"comment"},
			{eNodeType::Define,"define"},
			{eNodeType::Tree,"tree"},
			{eNodeType::Array,"array"},
			{eNodeType::Widget,"widget"},
		}
	);


// convert ePinType to string
string vpl::PinTypeToStr(const ePinType type)
{
	auto it = g_ePinTypeToStr.left.find(type);
	return (g_ePinTypeToStr.left.end() == it) ? "none" : it->second;
}


// convert string to ePinType
ePinType vpl::StrToPinType(const string& str)
{
	auto it = g_ePinTypeToStr.right.find(str);
	return (g_ePinTypeToStr.right.end() == it) ? ePinType::None : it->second;
}


// convert ePinKind to string
string vpl::PinKindToStr(const ePinKind kind)
{
	auto it = g_ePinKindToStr.left.find(kind);
	return (g_ePinKindToStr.left.end() == it) ? "output" : it->second;
}


// convert string to ePinKind
ePinKind vpl::StrToPinKind(const string& str)
{
	auto it = g_ePinKindToStr.right.find(str);
	return (g_ePinKindToStr.right.end() == it) ? ePinKind::Output : it->second;
}


// convert eNodeType to string
string vpl::NodeTypeToStr(const eNodeType type)
{
	auto it = g_eNodeTypeToStr.left.find(type);
	return (g_eNodeTypeToStr.left.end() == it) ? "none" : it->second;
}


// convert string to eNodeType
eNodeType vpl::StrToNodeType(const string& str)
{
	auto it = g_eNodeTypeToStr.right.find(str);
	return (g_eNodeTypeToStr.right.end() == it) ? eNodeType::None : it->second;
}



// Convert ePinType to Value Type
VARTYPE vpl::GetPin2VarType(const ePinType type)
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
		assert(!"vpl:::GetPin2VarType() Error symbol parse error!!");
		break;
	}
	return VT_EMPTY;
}


// is primitive type? bool, int, float, string
bool vpl::IsPrimitiveType(const ePinType type)
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

