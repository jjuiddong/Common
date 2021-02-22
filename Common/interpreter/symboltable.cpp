
#include "stdafx.h"
#include "symboltable.h"

using namespace common;
using namespace common::script;

std::atomic_int cSymbolTable::s_genId = 1;


//--------------------------------------------------------------------------
//cSymbolTable
//--------------------------------------------------------------------------
cSymbolTable::cSymbolTable()
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


// add or update variable in symboltable
bool cSymbolTable::Set(const string &scopeName, const string &symbolName
	, const variant_t &var, const string &typeStr //= ""
	)
{
	// to avoid bstr memory move bug
	common::clearvariant(m_vars[scopeName][symbolName].var);
	m_vars[scopeName][symbolName].var = var;
	m_vars[scopeName][symbolName].type = typeStr;
	return true;
}


// add or update array variable (empty array)
// var: only update array element type
bool cSymbolTable::SetArray(const string &scopeName, const string &symbolName
	, const variant_t &var
	, const string &typeStr //= ""
)
{
	sVariable arVar;
	arVar.type = "Array";

	// array tricky code, no memory allocate
	// no VT_ARRAY type, because reduce array copy time
	arVar.var.vt = VT_BYREF | var.vt;

	arVar.subType0 = var.vt; // array element type
	arVar.arSize = 0;
	arVar.ar = nullptr;
	m_vars[scopeName][symbolName] = arVar;

	sVariable &variable = m_vars[scopeName][symbolName];
	variable.var.intVal = variable.id;

	// add fast var search mapping
	m_varMap[variable.id] = { scopeName, symbolName };
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


sVariable* cSymbolTable::FindVarInfo(const string &scopeName
	, const string &symbolName)
{
	auto it = m_vars.find(scopeName);
	RETV(m_vars.end() == it, nullptr);

	auto it2 = it->second.find(symbolName);
	RETV(it->second.end() == it2, nullptr);

	return &it2->second;
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
	string scopeName = common::format("%s-%d", name.c_str(), id);
	return scopeName;
}


// parse scope name -> node name , node id
std::pair<string, int> cSymbolTable::ParseScopeName(const string &scopeName)
{
	vector<string> out;
	common::tokenizer(scopeName.c_str(), "-", "", out);
	if (out.size() < 2)
		return std::make_pair("", 0);
	return std::make_pair(out[0], atoi(out[1].c_str()));
}


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
				if (kv2.second.var.vt & VT_BYREF) // Array type?
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
	m_vars.clear();
	m_varMap.clear();

	for (auto &kv : m_symbols)
		delete kv.second;
	m_symbols.clear();
}
