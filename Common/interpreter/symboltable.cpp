
#include "stdafx.h"
#include "symboltable.h"

using namespace common;
using namespace common::script;

std::atomic_int cSymbolTable::s_genId = 1;


cSymbolTable::cSymbolTable()
	: m_isChange(false)
{
}
cSymbolTable::cSymbolTable(const cSymbolTable &rhs)
{
	operator=(rhs);
}
cSymbolTable::~cSymbolTable()
{
	Clear();
}


 
// add or update variable in symboltable (Set overloading)
bool cSymbolTable::Set(const string &scopeName, const string &symbolName
	, const variant_t &var
	, const string &typeStr //= ""
	)
{
	if (!IsExist(scopeName, symbolName))
		m_isChange = true;

	// to avoid bstr memory move bug
	common::clearvariant(m_vars[scopeName][symbolName].var);
	sVariable &variable = m_vars[scopeName][symbolName];

	// if already setting array or map, clear
	if ((var.vt & VT_BYREF) && (variable.ar || variable.m))
	{
		// check same variable
		if (var.intVal != variable.id)
		{
			variable.ClearArrayMemory();
			variable.ClearMapMemory();
		}
	}

	variable.type = typeStr;
	variable.var = var;
	ParseTypeString(typeStr, variable.typeValues);
	return true;
}


// add or update array variable (empty array)
// var: only update array element type
bool cSymbolTable::InitArray(const string &scopeName, const string &symbolName
	, const string &typeStr)
{
	if (!IsExist(scopeName, symbolName))
		m_isChange = true;

	// find exist array variable
	sVariable *arrayVar = nullptr;
	auto it1 = m_vars.find(scopeName);
	if (it1 != m_vars.end())
	{
		// already exist symbol?
		auto it2 = it1->second.find(symbolName);
		if (it2 != it1->second.end())
			arrayVar = &it2->second;
	}

	if (arrayVar && (eSymbolType::Array == arrayVar->typeValues[0]))
	{
		arrayVar->ClearArray();
	}
	else
	{
		sVariable newVar;
		newVar.type = "Array";
		common::script::ParseTypeString(typeStr, newVar.typeValues);
		newVar.subTypeStr = GenerateTypeString(newVar.typeValues, 1);
		// array tricky code, no memory allocate
		// no VT_ARRAY type, because reduce array copy time
		newVar.var.vt = VT_BYREF | VT_INT;
		newVar.arSize = 0;
		newVar.ar = nullptr;
		m_vars[scopeName][symbolName] = newVar;

		sVariable &variable = m_vars[scopeName][symbolName];
		variable.var.intVal = variable.id;

		// add fast var search mapping
		m_varMap[variable.id] = { scopeName, symbolName };
	}
	return true;
}


// copy array
// dest: scopeName + symbolName
// src: source array var
bool cSymbolTable::CopyArray(const string &scopeName, const string &symbolName
	, const variant_t &var )
{
	sVariable *dstVar = nullptr;
	auto it1 = m_vars.find(scopeName);
	if (it1 != m_vars.end())
	{
		// already exist symbol?
		auto it2 = it1->second.find(symbolName);
		if (it2 != it1->second.end())
			dstVar = &it2->second;
	}

	// find source array variable
	const int varId = var.intVal;
	script::sVariable *srcVar = nullptr;
	auto it = m_varMap.find(varId);
	if (m_varMap.end() == it)
		return false; // not found source array variable
	srcVar = FindVarInfo(it->second.first, it->second.second);
	if (!srcVar)
		return false; // error, not found source array

	if (dstVar)
	{
		*dstVar = *srcVar;
	}
	else
	{
		m_vars[scopeName][symbolName] = *srcVar;
		sVariable &dstVar = m_vars[scopeName][symbolName];

		// add fast var search mapping
		m_varMap[dstVar.id] = { scopeName, symbolName };
	}
	return true;
}


// array initializer
bool cSymbolTable::ArrayInitializer(const string& scopeName, const string& symbolName
	, const string& initStr)
{
	sVariable* var = FindRealVarInfo(scopeName, symbolName);
	if (!var) return false; // error return

	string valueStr = initStr;
	common::replaceAll(valueStr, "\"", "'");
	vector<string> toks;
	common::tokenizer(valueStr, ",", "", toks);

	const eSymbolType itemType = var->typeValues[1]; // array item type
	if (eSymbolType::None == itemType)
		return false; // not found array item type, error return
	
	var->ClearArray();
	variant_t val;
	for (auto& tok : toks)
	{
		common::replaceAll(tok, "'", ""); // remove \' character
		switch (itemType)
		{
		case eSymbolType::Bool: val = (bool)atoi(tok.c_str()); break;
		case eSymbolType::Enums:
		case eSymbolType::Int: val = (int)atoi(tok.c_str()); break;
		case eSymbolType::Float: val = (float)atof(tok.c_str()); break;
		case eSymbolType::String: val = common::str2variant(VT_BSTR, tok); break;
		default: continue;
		}
		var->PushArrayElement(val);
	}

	return true;
}


// add or update map variable (empty map)
// var: only update map element type
bool cSymbolTable::InitMap(const string &scopeName, const string &symbolName
	, const string &typeStr )
{
	if (!IsExist(scopeName, symbolName))
		m_isChange = true;

	sVariable *mapVar = nullptr;
	auto it1 = m_vars.find(scopeName);
	if (it1 != m_vars.end())
	{
		// already exist symbol?
		auto it2 = it1->second.find(symbolName);
		if (it2 != it1->second.end())
			mapVar = &it2->second;
	}

	if (mapVar && (eSymbolType::Map == mapVar->typeValues[0]))
	{
		mapVar->ClearMap();
	}
	else
	{
		sVariable newVar;
		//newVar.type = typeStr;
		newVar.type = "Map";
		common::script::ParseTypeString(typeStr, newVar.typeValues);
		newVar.subTypeStr = GenerateTypeString(newVar.typeValues, 2);
		newVar.var.vt = VT_BYREF | VT_INT; // map tricky code, no memory allocate

		m_vars[scopeName][symbolName] = newVar;

		sVariable &variable = m_vars[scopeName][symbolName];
		variable.var.intVal = variable.id;

		// add fast var search mapping
		m_varMap[variable.id] = { scopeName, symbolName };
	}
	return true;
}


// copy map
// dest: scopeName + symbolName
// src: var
bool cSymbolTable::CopyMap(const string &scopeName, const string &symbolName
	, const variant_t &var)
{
	sVariable *dstVar = nullptr;
	auto it1 = m_vars.find(scopeName);
	if (it1 != m_vars.end())
	{
		auto it2 = it1->second.find(symbolName);
		if (it2 != it1->second.end())
		{
			// already exist symbol
			dstVar = &it2->second;
		}
	}

	// find source array variable
	const int varId = var.intVal;
	script::sVariable *srcVar = nullptr;
	auto it = m_varMap.find(varId);
	if (m_varMap.end() == it)
		return false; // not found source map variable
	srcVar = FindVarInfo(it->second.first, it->second.second);
	if (!srcVar)
		return false; // error

	if (dstVar)
	{
		*dstVar = *srcVar;
	}
	else
	{
		m_vars[scopeName][symbolName] = *srcVar;
		sVariable &dstVar = m_vars[scopeName][symbolName];

		// add fast var search mapping
		m_varMap[dstVar.id] = { scopeName, symbolName };
	}
	return true;
}


// get variable in symboltable
bool cSymbolTable::Get(const string &scopeName, const string &symbolName
	, OUT variant_t &out)
{
	auto it = m_vars.find(scopeName);
	RETV(m_vars.end() == it, false);

	auto it2 = it->second.find(symbolName);
	RETV(it->second.end() == it2, false);

	out = it2->second.var;
	return true;
}


// convert variable value to string (only array type)
bool ConvertVariableToString(script::sVariable& var, OUT string &out)
{
	if (var.IsArray())
	{
		if (var.GetArraySize() == 0)
		{
			out = "[]";
			return true;
		}

		out = "[";
		switch (var.ar[0].vt)
		{
		case VT_BOOL:
			for (uint i=0; i < var.arSize; ++i)
				out += common::format("%d,", (bool)var.ar[i]);
			break;

		case VT_INT:
		case VT_R4:
			for (uint i=0; i < var.arSize; ++i)
				out += common::format("%f,", (float)var.ar[i]);
			break;

		case VT_BSTR:
			for (uint i=0; i < var.arSize; ++i)
			{
				const string str = (bstr_t)var.ar[i];
				out += str + ",";
			}
			break;

		default:
			break;
		}
		out += "]";
	}
	else
	{
		out = "null";
	}
	return true;
}


// get variable value to string
bool cSymbolTable::GetString(const string& scopeName, const string& symbolName
	, OUT string& out)
{
	sVariable *var = FindRealVarInfo(scopeName, symbolName);
	//RETV(!var, false);
	if (!var)
	{
		out = "null";
		return false;
	}

	switch (var->var.vt)
	{
	case VT_BOOL:
		out = common::format("%d", (bool)var->var);
		break;
	case VT_R4:
		out = common::format("%f", (float)var->var);
		break;
	case VT_BSTR:
		out = (const char*)(bstr_t)var->var;
		break;
	default:
	{
		if (var->IsArray())
		{
			ConvertVariableToString(*var, out);
		}
		else if (var->IsMap())
		{
			out = "null";
		}
	}
	break;
	}
	return true;
}


sVariable* cSymbolTable::FindVarInfo(const string &scopeName
	, const string &symbolName)
{
	auto it = m_vars.find(scopeName);
	RETV(m_vars.end() == it, nullptr);

	auto it2 = it->second.find(symbolName);
	RETV(it->second.end() == it2, nullptr);

	return &it2->second;
}


// find variable, if reference of array or map, find real variable and then return
sVariable* cSymbolTable::FindRealVarInfo(const string& scopeName, const string& symbolName)
{
	sVariable* var = FindVarInfo(scopeName, symbolName);
	RETV(!var, nullptr);
	if (!var->IsReference())
		return var;

	auto it = m_varMap.find(var->var.intVal);
	if (m_varMap.end() == it)
		return nullptr; // error return, not found real data

	var = FindVarInfo(it->second.first, it->second.second);
	return var;
}


bool cSymbolTable::IsExist(const string &scopeName, const string &symbolName)
{
	auto it = m_vars.find(scopeName);
	RETV(m_vars.end() == it, false);

	auto it2 = it->second.find(symbolName);
	RETV(it->second.end() == it2, false);

	return true;
}


// remove variable
bool cSymbolTable::RemoveVar(const string &scopeName, const string &symbolName)
{
	auto it = m_vars.find(scopeName);
	RETV(m_vars.end() == it, nullptr);
	
	// remove fast search mapping
	auto it2 = it->second.find(symbolName);
	if (it2 != it->second.end())
		m_varMap.erase(it2->second.id);

	// remove
	it->second.erase(symbolName);
	return true;
}


// add symbol
bool cSymbolTable::AddSymbol(const sSymbol &type)
{
	auto it = m_symbols.find(type.name);
	if (m_symbols.end() != it)
		return false; // already exist

	sSymbol *p = new sSymbol;
	*p = type;
	m_symbols.insert({ type.name, p });
	return true;
}


// remove symbol
bool cSymbolTable::RemoveSymbol(const string &typeName)
{
	auto it = m_symbols.find(typeName);
	if (m_symbols.end() == it)
		return false; // not exist

	delete it->second;
	m_symbols.erase(it);
	return true;
}


cSymbolTable::sSymbol* cSymbolTable::FindSymbol(const string &typeName)
{
	auto it = m_symbols.find(typeName);
	if (m_symbols.end() == it)
		return nullptr; // not exist
	return it->second;
}


// generate unique id
int cSymbolTable::GenID()
{
	return s_genId++;
}


// return scope name
// name: node name
// id: node id
string cSymbolTable::MakeScopeName(const string &name, const int id)
{
	string scopeName = common::format("%s_%d", name.c_str(), id);
	return scopeName;
}

// return scope name
// name: node name
// id: node id
// name2: pin name
string cSymbolTable::MakeScopeName2(const string &name, const int id, const string &name2)
{
	string scopeName = common::format("%s_%d_%s", name.c_str(), id, name2.c_str());
	return scopeName;
}

// return variable scope name
// scopeName: scope name
// varName: variable name in scope
string cSymbolTable::MakeScopeName3(const string& scopeName, const string& varName)
{
	string name = common::format("%s_%s", scopeName.c_str(), varName.c_str());
	return name;
}

// parse scope name -> node name , node id
std::pair<string, int> cSymbolTable::ParseScopeName(const string &scopeName)
{
	vector<string> out;
	common::tokenizer(scopeName.c_str(), "_", "", out);
	if (out.size() < 2)
		return std::make_pair("", 0);
	return std::make_pair(out[0], atoi(out[1].c_str()));
}

// parse scope name -> node name , node id
std::tuple<string, int, string> cSymbolTable::ParseScopeName2(const string &scopeName)
{
	vector<string> out;
	common::tokenizer(scopeName.c_str(), "_", "", out);
	if (out.size() < 2)
		return std::make_tuple("", 0, "");
	if (out.size() < 3)
		return std::make_tuple(out[0], atoi(out[1].c_str()), "");
	return std::make_tuple(out[0], atoi(out[1].c_str()), out[2]);
}


// assign operator
cSymbolTable& cSymbolTable::operator=(const cSymbolTable &rhs)
{
	if (this != &rhs)
	{
		Clear();

		// copy all symbols
		for (auto &kv1 : rhs.m_vars)
			for (auto &kv2 : kv1.second)
				m_vars[kv1.first][kv2.first] = kv2.second;

		// update fast var search mapping
		m_varMap.clear();
		for (auto &kv : m_vars) 
			for (auto &kv2 : kv.second)
				if (kv2.second.var.vt & VT_BYREF) // Array, Map type?
					m_varMap[kv2.second.id] = { kv.first, kv2.first }; // scopeName,varName

		for (auto &kv1 : rhs.m_symbols)
		{
			sSymbol *sym = new sSymbol;
			*sym = *kv1.second;
			m_symbols[kv1.first] = sym;
		}
	}
	return *this;
}


// copy only symbol data
bool cSymbolTable::CopySymbols(const cSymbolTable &rhs)
{
	if (this == &rhs)
		return false;

	for (auto &kv : m_symbols)
		delete kv.second;
	m_symbols.clear();

	for (auto &kv1 : rhs.m_symbols)
	{
		sSymbol *sym = new sSymbol;
		*sym = *kv1.second;
		m_symbols[kv1.first] = sym;
	}
	return true;
}


void cSymbolTable::Clear()
{
	m_isChange = false;
	m_vars.clear();
	m_varMap.clear();

	for (auto &kv : m_symbols)
		delete kv.second;
	m_symbols.clear();
}
