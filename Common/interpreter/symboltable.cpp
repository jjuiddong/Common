
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
bool cSymbolTable::Set(const string &scopeName, const string &symbolName
	, const variant_t &var)
{
	variant_t tmp = var; // to avoid bstr memory move bug
	m_symbols[scopeName][symbolName] = tmp;
	return true;
}


// get variable in symboltable
bool cSymbolTable::Get(const string &scopeName, const string &symbolName
	, OUT variant_t &out)
{
	auto it = m_symbols.find(scopeName);
	RETV(m_symbols.end() == it, false);

	auto it2 = it->second.find(symbolName);
	RETV(it->second.end() == it2, false);

	out = it2->second;
	return true;
}


bool cSymbolTable::IsExist(const string &scopeName, const string &symbolName)
{
	auto it = m_symbols.find(scopeName);
	RETV(m_symbols.end() == it, false);

	auto it2 = it->second.find(symbolName);
	RETV(it->second.end() == it2, false);

	return true;
}


// return scope name
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


void cSymbolTable::Clear()
{
	m_symbols.clear();
}
