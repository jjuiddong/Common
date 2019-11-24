
#include "stdafx.h"
#include "typetable.h"

using namespace common;
using namespace common::script;


cTypeTable::cTypeTable()
{
}

cTypeTable::cTypeTable(const cTypeTable &rhs)
{
	operator=(rhs);
}

cTypeTable::~cTypeTable()
{
	Clear();
}


bool cTypeTable::AddType(const sType &type) 
{
	auto it = m_types.find(type.name);
	if (m_types.end() != it)
		return false; // already exist

	sType *p = new sType;
	*p = type;
	m_types.insert({ type.name, p });
	return true;
}


bool cTypeTable::RemoveType(const string &typeName)
{
	auto it = m_types.find(typeName);
	if (m_types.end() == it)
		return false; // not exist

	delete it->second;
	m_types.erase(it);
	return true;
}


cTypeTable::sType* cTypeTable::FindType(const string &typeName)
{
	auto it = m_types.find(typeName);
	if (m_types.end() == it)
		return nullptr; // not exist
	return it->second;
}


cTypeTable& cTypeTable::operator=(const cTypeTable &rhs)
{
	if (this != &rhs)
	{
		Clear();

		// deep copy
		for (auto &kv : rhs.m_types)
		{
			sType t;
			t.name = kv.first;
			t.enums = kv.second->enums;
			AddType(t);
		}
	}
	return *this;
}


void cTypeTable::Clear()
{
	for (auto &kv : m_types)
		delete kv.second;
	m_types.clear();
}
