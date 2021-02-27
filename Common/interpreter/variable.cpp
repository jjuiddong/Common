
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
	, id(cSymbolTable::GenID())
	, arSize(0)
	, arCapacity(0)
	, subType0(0)
	, subType1(0)
{
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
	switch (subType0)
	{
	case VT_BOOL: return g_emptyBool;
	case VT_INT: return g_emptyInt;
	case VT_R4: return g_emptyFloat;
	case VT_BSTR: return g_emptyString;
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
		switch (subType0)
		{
		case VT_BOOL: return g_emptyBool;
		case VT_INT: return g_emptyInt;
		case VT_R4: return g_emptyFloat;
		case VT_BSTR: return g_emptyString;
		default: return g_emptyInt;
		}
	}
}


// reserve array memory
bool sVariable::ReserveArray(const uint size)
{
	if (arCapacity > size)
		return false;

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


// assign operator
// array shallow copy
sVariable& sVariable::operator=(const sVariable &rhs) 
{
	if (this != &rhs)
	{
		ClearArray();
		type = rhs.type;
		var = rhs.var;
		subType0 = rhs.subType0;
		subType1 = rhs.subType1;

		// copy array, deep copy
		// arraytype? intVal is sVar id
		if (rhs.var.vt & VT_BYREF)
			var.intVal = id;

		if (rhs.arSize > 0) {
			ReserveArray(rhs.arSize);
			for (uint i = 0; i < rhs.arSize; ++i)
				ar[i] = rhs.ar[i];
			arSize = rhs.arSize;
		}
	}
	return *this;
}


// clear variable, delete allocated memory 
void sVariable::Clear() 
{
	common::clearvariant(var);
	ClearArrayMemory();
}


// clear array
void sVariable::ClearArray()
{
	arSize = 0;
}


// clear array allocated memory
void sVariable::ClearArrayMemory()
{
	for (uint i = 0; i < arSize; ++i)
		common::clearvariant(ar[i]);
	SAFE_DELETEA(ar);
	arSize = 0;
	arCapacity = 0;
}
