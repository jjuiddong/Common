
#include "stdafx.h"
#include "symboltable.h"

using namespace framework;
using namespace vprog;


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
	VARTYPE vt = VT_VOID;
	switch (pin.type)
	{
	case ePinType::Bool: vt = VT_BOOL; break;
	case ePinType::Int: vt = VT_INT; break;
	case ePinType::Float: vt = VT_R4; break;
	case ePinType::String: vt = VT_BSTR; break;
	default:
		assert(!"nodefile::AddSymbol() symbol parse error!!");
		break;
	}

	sValue v;
	v.str = value;
	v.var = common::str2variant(vt, v.str);
	v.var.vt = vt;
	m_symbols.insert({ pin.id.Get(), v });
	return true;
}


// add variant type symbol
bool cSymbolTable::AddSymbol(const sPin &pin, const variant_t &value)
{
	VARTYPE vt;
	switch (pin.type)
	{
	case ePinType::Bool: vt = VT_BOOL; break;
	case ePinType::Int: vt = VT_INT; break;
	case ePinType::Float: vt = VT_R4; break;
	case ePinType::String: vt = VT_BSTR; break;
	default:
		assert(!"nodefile::AddSymbol() symbol parse error!!");
		break;
	}

	sValue v;
	v.var = value;
	v.str = common::variant2str(value);
	if (ePinType::String == pin.type)
		v.var = common::str2variant(vt, v.str); // for string pointer
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
