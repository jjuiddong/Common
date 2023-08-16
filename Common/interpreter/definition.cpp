
#include "stdafx.h"
#include "definition.h"

using namespace common;
using namespace common::script;


static boost::bimap<eCommand, string> g_eCommandToStr =
	make_bimap<eCommand, string>(
		{
			{eCommand::none,"none"},
			{eCommand::ldbc,"ldbc"},
			{eCommand::ldic,"ldic"},
			{eCommand::ldfc,"ldfc"},
			{eCommand::ldsc,"ldsc"},
			{eCommand::ldac,"ldac"},
			{eCommand::ldmc,"ldmc"},
			{eCommand::ldcmp,"ldcmp"},
			{eCommand::ldncmp,"ldncmp"},
			{eCommand::ldtim,"ldtim"},
			{eCommand::getb,"getb"},
			{eCommand::geti,"geti"},
			{eCommand::getf,"getf"},
			{eCommand::gets,"gets"},
			{eCommand::geta,"geta"},
			{eCommand::getm,"getm"},
			{eCommand::setb,"setb"},
			{eCommand::seti,"seti"},
			{eCommand::setf,"setf"},
			{eCommand::sets,"sets"},
			{eCommand::seta,"seta"},
			{eCommand::setm,"setm"},
			{eCommand::copya,"copya"},
			{eCommand::copym,"copym"},
			{eCommand::addi,"addi"},
			{eCommand::addf,"addf"},
			{eCommand::adds,"adds"},
			{eCommand::subi,"subi"},
			{eCommand::subf,"subf"},
			{eCommand::muli,"muli"},
			{eCommand::mulf,"mulf"},
			{eCommand::divi,"divi"},
			{eCommand::divf,"divf"},
			{eCommand::remi,"remi"},
			{eCommand::remf,"remf"},
			{eCommand::opand,"opand"},
			{eCommand::opor,"opor"},
			{eCommand::negate,"negate"},
			{eCommand::eqi,"eqi"},
			{eCommand::eqf,"eqf"},
			{eCommand::eqs,"eqs"},
			{eCommand::eqic,"eqic"},
			{eCommand::eqfc,"eqfc"},
			{eCommand::eqsc,"eqsc"},
			{eCommand::lesi,"lesi"},
			{eCommand::lesf,"lesf"},
			{eCommand::leqi,"leqi"},
			{eCommand::leqf,"leqf"},
			{eCommand::gri,"gri"},
			{eCommand::grf,"grf"},
			{eCommand::greqi,"greqi"},
			{eCommand::greqf,"greqf"},
			{eCommand::call,"call"},
			{eCommand::jnz,"jnz"},
			{eCommand::jmp,"jmp"},
			{eCommand::label,"label"},
			{eCommand::pushic,"pushic"},
			{eCommand::pop,"pop"},
			{eCommand::sret,"sret"},
			{eCommand::cstack,"cstack"},
			{eCommand::synct,"synct"},
			{eCommand::synci,"synci"},
			{eCommand::sync,"sync"},
			{eCommand::symbolb,"symbolb"},
			{eCommand::symboli,"symboli"},
			{eCommand::symbolf,"symbolf"},
			{eCommand::symbols,"symbols"},
			{eCommand::symbolab,"symbolab"},
			{eCommand::symbolai,"symbolai"},
			{eCommand::symbolaf,"symbolaf"},
			{eCommand::symbolas,"symbolas"},
			{eCommand::symbolmb,"symbolmb"},
			{eCommand::symbolmi,"symbolmi"},
			{eCommand::symbolmf,"symbolmf"},
			{eCommand::symbolms,"symbolms"},
			{eCommand::symbolma,"symbolma"},
			{eCommand::timer1,"timer1"},
			{eCommand::timer2,"timer2"},
			{eCommand::cmt,"cmt"},
			{eCommand::delay,"delay"},
			{eCommand::nop,"nop"},
		}
	);

static boost::bimap<eSymbolType, string> g_eSymbolTypeToStr =
	make_bimap<eSymbolType, string>(
		{
			{eSymbolType::None, "none"},
			{eSymbolType::Bool, "bool"},
			{eSymbolType::Int, "int"},
			{eSymbolType::Float, "float"},
			{eSymbolType::Enums, "enums"},
			{eSymbolType::String, "string"},
			{eSymbolType::Array, "array"},
			{eSymbolType::Map, "map"},
			{eSymbolType::Any, "any"},
		}
	);

// convert eCommand to string
string script::CommandToStr(const eCommand cmd)
{
	auto it = g_eCommandToStr.left.find(cmd);
	return (g_eCommandToStr.left.end() == it)? "none" : it->second;
}


// convert string to eCommand
eCommand script::StrToCommand(const string& str)
{
	auto it = g_eCommandToStr.right.find(str);
	return (g_eCommandToStr.right.end() == it) ? eCommand::none : it->second;
}


// return VarType corresspond eCommand
VARTYPE script::GetVarType(const eCommand cmd)
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
	case eCommand::remi:
	case eCommand::eqi:
	case eCommand::lesi:
	case eCommand::eqic:
	case eCommand::leqi:
	case eCommand::gri:
	case eCommand::greqi:
	case eCommand::symboli:
	case eCommand::pushic:
	case eCommand::timer1:
	case eCommand::timer2:
	case eCommand::synct:
	case eCommand::synci:
	case eCommand::sync:
		return VT_INT;

	case eCommand::ldfc:
	case eCommand::ldtim:
	case eCommand::getf:
	case eCommand::setf:
	case eCommand::addf:
	case eCommand::subf:
	case eCommand::mulf:
	case eCommand::divf:
	case eCommand::remf:
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
	case eCommand::negate:
	case eCommand::opand:
	case eCommand::opor:
		return VT_BOOL;

	case eCommand::ldac:
	case eCommand::geta:
	case eCommand::seta:
	case eCommand::copya:
	case eCommand::symbolab:
	case eCommand::symbolai:
	case eCommand::symbolaf:
	case eCommand::symbolas:
	case eCommand::ldmc:
	case eCommand::getm:
	case eCommand::setm:
	case eCommand::copym:
	case eCommand::symbolmb:
	case eCommand::symbolmi:
	case eCommand::symbolmf:
	case eCommand::symbolms:
	case eCommand::symbolma:
		return VT_BYREF | VT_INT; // array, map type, tricky code

	case eCommand::call:
	case eCommand::jnz:
	case eCommand::jmp:
	case eCommand::label:
	case eCommand::sret:
	case eCommand::pop:
	case eCommand::cstack:
	case eCommand::nop:
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
		str3 = rhs.str3;
		reg1 = rhs.reg1;
		reg2 = rhs.reg2;
		common::clearvariant(var1);
		var1 = common::copyvariant(rhs.var1);
	}
	return *this;
}


//----------------------------------------------------------------
// eSymbolType global functions

string script::SymbolTypeToStr(const eSymbolType& symbType)
{
	auto it = g_eSymbolTypeToStr.left.find(symbType);
	return (g_eSymbolTypeToStr.left.end() == it) ? "none" : it->second;
}


eSymbolType script::StrToSymbolType(const string& str)
{
	auto it = g_eSymbolTypeToStr.right.find(common::lowerCase2(str));
	return (g_eSymbolTypeToStr.right.end() == it) ? eSymbolType::Enums : it->second;
}


// is variable type?
bool script::IsVariable(const eSymbolType type)
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


// convert type string to VarType Array
// type string -> B[b]ool | I[i]nt | S[s]tring | F[f]loat | A[a]rray | M[m]ap
// Array<type string>, Map<type string, type string>
// ex) Map<string, Array<string>>
//      - return: [Map, String, Array, String]
bool script::ParseTypeString(const string &typeStr, OUT vector<eSymbolType> &out)
{
	const int idx0 = typeStr.find('>');
	const int idx1 = typeStr.find('<');
	const int idx = ((idx0 < 0) && (idx1 < 0)) ? -1 :
		(((idx0 < 0) || (idx1 < 0)) ? max(idx0, idx1)
			: min(idx0, idx1));

	if (idx < 0)
	{
		vector<string> strs;
		common::tokenizer(typeStr, ",", "", strs);
		for (auto &s : strs)
		{
			common::trim(s);
			if (s.empty())
				continue;
			const eSymbolType st = StrToSymbolType(s);
			out.push_back(st);
		}
	}
	else
	{
		const string tstr = common::trim2(typeStr.substr(0, idx));
		ParseTypeString(tstr, out);
		const string str = common::trim2(typeStr.substr(idx + 1));
		ParseTypeString(str, out);
	}
	return !out.empty();
}


// parse type string, array version
bool script::ParseTypeString(const string &typeStr, OUT eSymbolType out[4])
{
	vector<eSymbolType> temp;
	ParseTypeString(typeStr, temp);

	for (uint i = 0; i < 4; ++i)
		out[i] = eSymbolType::None;
	for (uint i = 0; (i < 4) && (i < temp.size()); ++i)
		out[i] = temp[i];
	
	return !temp.empty();
}


// generate typestring from type array
// idx: in/out index
string GetTypeString(const vector<eSymbolType> &typeValues, INOUT uint &idx
	, const uint size)
{
	if (typeValues.size() <= idx)
		return "";

	string typeStr;
	for (uint i = 0; i < size; ++i)
	{
		if (typeValues.size() <= idx)
			return typeStr;

		if (i > 0)
			typeStr += ",";

		const eSymbolType type = typeValues[idx];
		switch (type)
		{
		case eSymbolType::Map:
			typeStr += "map<";
			typeStr += GetTypeString(typeValues, ++idx, 2);
			typeStr += ">";
			break;
		case eSymbolType::Array:
			typeStr += "array<";
			typeStr += GetTypeString(typeValues, ++idx, 1);
			typeStr += ">";
			break;
		case eSymbolType::Enums:
		default:
			typeStr += SymbolTypeToStr(type);
			++idx;
			break;
		}
	}
	return typeStr;
}


// generate typestring from type vector
// ex) [eSymbolType::Array, eSymbolType::Int] => "array<Int>"
string script::GenerateTypeString(const vector<eSymbolType> &typeValues)
{
	uint idx = 0;
	const string typeStr = GetTypeString(typeValues, idx, 1);
	return typeStr;
}


// generate typestring from type array
string script::GenerateTypeString(const eSymbolType typeValues[4]
	, const uint startIdx //=0
)
{
	vector<eSymbolType> temp;
	for (uint i = startIdx; i < 4; ++i)
	{
		if (eSymbolType::None == typeValues[i])
			break;
		temp.push_back(typeValues[i]);
	}
	return GenerateTypeString(temp);
}
