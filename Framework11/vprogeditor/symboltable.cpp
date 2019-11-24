
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
		common::clearvariant(var);
		var = common::copyvariant(rhs.var);
		str = rhs.str;
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
bool cSymbolTable::AddSymbolStr(const sPin &pin, const string &value)
{
	const VARTYPE vt = vprog::GetPin2VarType(pin.type);

	sValue v;
	v.str = value;
	v.var = common::str2variant(vt, v.str);
	m_symbols.insert({ pin.id.Get(), v });
	return true;
}


// add variant type symbol
bool cSymbolTable::AddSymbol(const sPin &pin, const variant_t &value)
{
	const VARTYPE vt = vprog::GetPin2VarType(pin.type);

	sValue v;
	v.var = copyvariant(value);
	v.str = common::variant2str(value);
	m_symbols.insert({ pin.id.Get(), v });
	return true;
}


cSymbolTable::sValue* cSymbolTable::FindSymbol(const ed::PinId id)
{
	auto it = m_symbols.find(id.Get());
	if (m_symbols.end() == it)
		return NULL;
	return &it->second;
}


void cSymbolTable::Clear()
{
	m_symbols.clear();
}
