
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
	_variant_t varType;
	switch (pin.type)
	{
	case ePinType::Bool: varType.vt = VT_BOOL; break;
	case ePinType::Int: varType.vt = VT_I4; break;
	case ePinType::Float: varType.vt = VT_R4; break;
	case ePinType::String: varType.vt = VT_BSTR; break;
	default:
		assert(!"nodefile::AddSymbol() symbol parse error!!");
		break;
	}

	sValue v;
	v.str = value;
	v.var = common::str2variant(varType, v.str);
	v.var.vt = varType.vt;
	m_symbols.insert({ pin.id.Get(), v });
	return true;
}


// add variant type symbol
bool cSymbolTable::AddSymbol(const sPin &pin, const variant_t &value)
{
	_variant_t varType;
	switch (pin.type)
	{
	case ePinType::Bool: varType.vt = VT_BOOL; break;
	case ePinType::Int: varType.vt = VT_I4; break;
	case ePinType::Float: varType.vt = VT_R4; break;
	case ePinType::String: varType.vt = VT_BSTR; break;
	default:
		assert(!"nodefile::AddSymbol() symbol parse error!!");
		break;
	}

	sValue v;
	v.var = value;
	v.str = common::variant2str(value);
	if (ePinType::String == pin.type)
		v.var = common::str2variant(varType, v.str); // for string pointer
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
