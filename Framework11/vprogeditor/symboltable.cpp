
#include "stdafx.h"
#include "symboltable.h"

using namespace framework;
using namespace vprog;


//-------------------------------------------------------------------
// cSymbolTable::sValue
//-------------------------------------------------------------------
cSymbolTable::sValue::sValue()
{
}
cSymbolTable::sValue::~sValue()
{
	common::clearvariant(var);
}
cSymbolTable::sValue::sValue(const sValue &rhs)
{
	operator=(rhs);
}
cSymbolTable::sValue& cSymbolTable::sValue::operator=(const sValue &rhs)
{
	if (this != &rhs)
	{
		type = rhs.type;
		str = rhs.str;
		common::clearvariant(var);
		var = common::copyvariant(rhs.var);
	}
	return *this;
}


//-------------------------------------------------------------------
// cSymbolTable
//-------------------------------------------------------------------
cSymbolTable::cSymbolTable()
{
}

cSymbolTable::~cSymbolTable()
{
	Clear();
}


// add string type symbol
bool cSymbolTable::AddVarStr(const sPin &pin, const string &value
	, const string &type //= ""
)
{
	const VARTYPE vt = vprog::GetPin2VarType(pin.type);

	sValue v;
	v.type = type;
	v.str = value;
	v.var = common::str2variant(vt, v.str);
	m_vars.insert({ pin.id.Get(), v });
	return true;
}


// add variant type symbol
bool cSymbolTable::AddVar(const sPin &pin, const variant_t &value
	, const string &type //= ""
)
{
	sValue v;
	v.type = type;
	v.var = copyvariant(value);
	v.str = common::variant2str(value);
	m_vars.insert({ pin.id.Get(), v });
	return true;
}


cSymbolTable::sValue* cSymbolTable::FindVar(const ed::PinId id)
{
	auto it = m_vars.find(id.Get());
	if (m_vars.end() == it)
		return NULL;
	return &it->second;
}


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


cSymbolTable& cSymbolTable::operator=(const cSymbolTable &rhs)
{
	if (this != &rhs)
	{
		Clear();

		// copy all symbols, variable
		for (auto &kv1 : rhs.m_vars)
			m_vars[kv1.first] = kv1.second;

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

	for (auto &kv : m_symbols)
		delete kv.second;
	m_symbols.clear();

}
