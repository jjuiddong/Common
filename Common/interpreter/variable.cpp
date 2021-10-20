
#include "stdafx.h"
#include "variable.h"

using namespace common;
using namespace script;

namespace
{
	// exception variable
	static variant_t g_emptyBool = (bool)false;
	static variant_t g_emptyInt = (int)0;
	static variant_t g_emptyFloat = (float)0.f;
	static variant_t g_emptyString = "";
}


sVariable::sVariable()
	: ar(nullptr)
	, m(nullptr)
	, id(cSymbolTable::GenID())
	, arSize(0)
	, arCapacity(0)
{
	ZeroMemory(typeValues, sizeof(typeValues));
}

sVariable::sVariable(const sVariable &rhs) 
{
	operator=(rhs);
}

sVariable::~sVariable() 
{
	Clear();
}


// return ar[index]
variant_t& sVariable::GetArrayElement(const uint index)
{
	if (arSize > index)
		return ar[index];

	// except process
	switch (typeValues[1])
	{
	case eSymbolType::Bool: return g_emptyBool;
	case eSymbolType::Int: return g_emptyInt;
	case eSymbolType::Float: return g_emptyFloat;
	case eSymbolType::String: return g_emptyString;
	default: return g_emptyInt;
	}
}


// set ar[index] = v
bool sVariable::SetArrayElement(const uint index, const variant_t &v)
{
	if (arSize > index)
	{
		ar[index] = v;
		return true;
	}
	return false;
}


// push 
bool sVariable::PushArrayElement(const variant_t &v)
{
	if (arSize < arCapacity)
	{
		ar[arSize++] = v;
	}
	else
	{
		const uint size = (arSize == 0) ? 1 : arSize * 2;
		ReserveArray(size);
		ar[arSize++] = v;
	}
	return true;
}


// pop
variant_t& sVariable::PopArrayElement()
{
	if (arSize > 0)
	{
		return ar[--arSize];
	}
	else
	{
		// except process
		switch (typeValues[1])
		{
		case eSymbolType::Bool: return g_emptyBool;
		case eSymbolType::Int: return g_emptyInt;
		case eSymbolType::Float: return g_emptyFloat;
		case eSymbolType::String: return g_emptyString;
		default: return g_emptyInt;
		}
	}
}


// reserve array memory
bool sVariable::ReserveArray(const uint size)
{
	if (arCapacity >= size)
		return true;

	variant_t *newAr = new variant_t[size];
	for (uint i = 0; i < arSize; ++i)
		newAr[i] = ar[i];
	const uint tmp = arSize;
	ClearArrayMemory();
	ar = newAr;
	arSize = tmp;
	arCapacity = size;
	return true;
}


// return array size
uint sVariable::GetArraySize()
{
	return arSize;
}


// return m[key]
// no data? create new data
variant_t& sVariable::GetMapValue(cSymbolTable &symbolTable, const string &key)
{
	if (m)
	{
		auto it = m->find(key);
		if (m->end() != it)
			return it->second;
	}

	// except process
	switch (typeValues[2]) // get map value type
	{
	case eSymbolType::Bool: return g_emptyBool;
	case eSymbolType::Int: return g_emptyInt;
	case eSymbolType::Float: return g_emptyFloat;
	case eSymbolType::String: return g_emptyString;
	case eSymbolType::Array:
	{
		// check current variable available
		auto it = symbolTable.m_varMap.find(id);
		if (symbolTable.m_varMap.end() == it)
			return g_emptyInt; // error

		// find source map variable
		const string scopeName = it->second.first;
		const string varName = it->second.second;
		sVariable *mapVar = symbolTable.FindVarInfo(scopeName, varName);
		if (!mapVar)
			return g_emptyInt; // error

		// create new array
		stringstream ss;
		ss << varName << "[" << key << "]";
		const string newVarName = ss.str(); // array key
		symbolTable.InitArray(scopeName, newVarName, subTypeStr);

		// get creation array
		sVariable *arVar = symbolTable.FindVarInfo(scopeName, newVarName);
		if (!arVar)
			return g_emptyInt; // error
		if (!m)
			m = new map<string, variant_t>();
		(*m)[key] = arVar->var;
		return arVar->var;
	}
	default: return g_emptyInt;
	}
}


// return map value type string
const string& sVariable::GetMapValueTypeStr()
{
	return subTypeStr;
}

// return array element type string
const string& sVariable::GetArrayElementTypeStr()
{
	return subTypeStr;
}


// set m[key] = v
bool sVariable::SetMapValue(const string &key, const variant_t &v)
{
	if (!m)
		m = new map<string, variant_t>();
	if (m)
	{
		(*m)[key] = v;
		return true;
	}
	return false;
}


// has m[key]?
bool sVariable::HasMapValue(const string &key)
{
	if (m)
	{
		auto it = m->find(key);
		return (m->end() != it);
	}
	return false;
}


// return map size
uint sVariable::GetMapSize()
{
	if (m)
		return m->size();
	return 0;
}


// assign operator
// array deep copy
sVariable& sVariable::operator=(const sVariable &rhs) 
{
	if (this != &rhs)
	{
		ClearArray();
		type = rhs.type;
		subTypeStr = rhs.subTypeStr;
		var = rhs.var; // variable or array, map type id assign
		memcpy(typeValues, rhs.typeValues, sizeof(rhs.typeValues));

		// copy array, deep copy
		// array or map type? intVal is sVariable id
		if (rhs.var.vt & VT_BYREF)
			var.intVal = id;

		// array copy
		if ((rhs.type == "Array") || (rhs.type == "array"))
		{
			if (rhs.arSize > 0) 
			{
				ReserveArray(rhs.arSize);
				for (uint i = 0; i < rhs.arSize; ++i)
					ar[i] = rhs.ar[i];
				arSize = rhs.arSize;
			}
		}
		// map copy
		else if ((rhs.type == "Map") || (rhs.type == "map"))
		{
			ClearMap();
			if (m) 
				m->clear();
			if (rhs.m)
			{
				if (!m)
					m = new map<string, variant_t>();
				m->clear();
				for (auto &kv : *rhs.m)
					(*m)[kv.first] = kv.second;
			}
		}
	}
	return *this;
}


// clear variable, delete allocated memory 
void sVariable::Clear() 
{
	common::clearvariant(var);
	ClearArrayMemory();
	ClearMapMemory();
}


// clear array
void sVariable::ClearArray()
{
	arSize = 0;
}


// clear array allocated memory
void sVariable::ClearArrayMemory()
{
	SAFE_DELETEA(ar);
	arSize = 0;
	arCapacity = 0;
}


// clear map
void sVariable::ClearMap()
{
	if (m)
		m->clear();
}


// clear map allocated memory
void sVariable::ClearMapMemory()
{
	SAFE_DELETE(m);
}

