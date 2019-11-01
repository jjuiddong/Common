
#include "stdafx.h"
#include "symboltable.h"

using namespace common;
using namespace common::script;


cSymbolTable::cSymbolTable()
{
}

cSymbolTable::~cSymbolTable()
{
	Clear();
}


// add or update variable in symboltable
bool cSymbolTable::Set(const StrId &scopeName, const StrId &symbolName
	, const variant_t &var)
{
	m_symbols[scopeName][symbolName] = var;

	// string table for variant_t bstr type
	if (VT_BSTR == var.vt)
	{
		const StrId id = scopeName + "::" + symbolName;
		m_strTable[id] = (LPCTSTR)var.pbstrVal; // copy string to management memory
		// update bstr pointer to management memory pointer
		m_symbols[scopeName][symbolName].pbstrVal = (BSTR*)m_strTable[id].c_str();
	}

	return true;
}


// get variable in symboltable
bool cSymbolTable::Get(const StrId &scopeName, const StrId &symbolName
	, OUT variant_t &out)
{
	auto it = m_symbols.find(scopeName);
	RETV(m_symbols.end() == it, false);

	auto it2 = it->second.find(symbolName);
	RETV(it->second.end() == it2, false);

	out = it2->second;
	return true;
}


bool cSymbolTable::IsExist(const StrId &scopeName, const StrId &symbolName)
{
	auto it = m_symbols.find(scopeName);
	RETV(m_symbols.end() == it, false);

	auto it2 = it->second.find(symbolName);
	RETV(it->second.end() == it2, false);

	return true;
}


void cSymbolTable::Clear()
{
	m_symbols.clear();
	m_strTable.clear();
}
