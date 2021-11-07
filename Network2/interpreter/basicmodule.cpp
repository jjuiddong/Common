
#include "stdafx.h"
#include "basicmodule.h"

using namespace network2;
using namespace common::script;


cBasicModule::cBasicModule()
{
}

cBasicModule::~cBasicModule()
{
}


// execute script function
eModuleResult cBasicModule::Execute(cVirtualMachine &vm
	, const string &scopeName, const string &funcName)
{
	script::cSymbolTable &symbolTable = vm.m_symbTable;

	if (funcName == "ToString")
	{
		script::sVariable *var = symbolTable.FindVarInfo(scopeName, "in");
		if (var)
		{
			const string str = common::variant2str(var->var);
			symbolTable.Set<string>(scopeName, "out", str, "string");
		}
		else
		{
			// exception process
			symbolTable.Set<string>(scopeName, "out", "null", "string");
		}
		return eModuleResult::Done;
	}
	else if (funcName == "ToNumber")
	{
		script::sVariable *var = symbolTable.FindVarInfo(scopeName, "string");
		if (var)
		{
			const string str = common::variant2str(var->var);
			symbolTable.Set<float>(scopeName, "out", (float)atof(str.c_str()), "float");
		}
		else
		{
			// exception process
			symbolTable.Set<float>(scopeName, "out", 0.f, "float");
		}
		return eModuleResult::Done;
	}
	else if (funcName == "ToBoolean")
	{
		script::sVariable *var = symbolTable.FindVarInfo(scopeName, "string");
		if (var)
		{
			const string str = common::trim2(common::variant2str(var->var));
			const bool res = str.empty() || (str == "0") || (str == "false")
				|| (str == "FALSE");
			symbolTable.Set<bool>(scopeName, "out", !res, "bool");
		}
		else
		{
			// exception process
			symbolTable.Set<bool>(scopeName, "out", false, "bool");
		}
		return eModuleResult::Done;
	}
	else if (funcName == "Floor")
	{
		const float value = symbolTable.Get<float>(scopeName, "number");
		symbolTable.Set<float>(scopeName, "out", floor(value), "float");
		return eModuleResult::Done;
	}
	else if (funcName == "Ceil")
	{
		const float value = symbolTable.Get<float>(scopeName, "number");
		symbolTable.Set<float>(scopeName, "out", ceil(value), "float");
		return eModuleResult::Done;
	}
	else if (funcName == "Round")
	{
		const float value = symbolTable.Get<float>(scopeName, "number");
		symbolTable.Set<float>(scopeName, "out", round(value), "float");
		return eModuleResult::Done;
	}
	else if (funcName == "Stop Timer")
	{
		const int id = symbolTable.Get<int>(scopeName, "id");
		const bool result = vm.StopTimer(id);
		symbolTable.Set<bool>(scopeName, "result", result, "bool");
	}
	else if ((funcName == "Array.Get")
		|| (funcName == "Array.Set")
		|| (funcName == "Array.Push")
		|| (funcName == "Array.Pop")
		|| (funcName == "Array.Size")
		|| (funcName == "Array.Empty")
		|| (funcName == "Array.Clear")
		|| (funcName == "Array.Clone"))
	{
		ArrayFunction(vm, scopeName, funcName);
		return eModuleResult::Done;
	}
	else if ((funcName == "Map.Get")
		|| (funcName == "Map.Set")
		|| (funcName == "Map.Has")
		|| (funcName == "Map.Size")
		|| (funcName == "Map.Empty")
		|| (funcName == "Map.Clear")
		|| (funcName == "Map.Clone"))
	{
		MapFunction(vm, scopeName, funcName);
		return eModuleResult::Done;
	}

	return eModuleResult::NoHandler;
}


// Array.Get/Set/Push/Pop/Size function process
bool cBasicModule::ArrayFunction(script::cVirtualMachine &vm
	, const string &scopeName, const string &funcName)
{
	script::cSymbolTable &symbolTable = vm.m_symbTable;
	script::sVariable *var = symbolTable.FindVarInfo(scopeName, "array");
	if (!var) {
		symbolTable.Set<bool>(scopeName, "out", false);
		return true; // error, not found array
	}

	int index = 0;
	const int varId = var->var.intVal;
	script::sVariable *srcVar = nullptr;

	auto it = symbolTable.m_varMap.find(varId);
	if (symbolTable.m_varMap.end() == it)
		goto $error;

	// find source array variable
	srcVar = symbolTable.FindVarInfo(it->second.first, it->second.second);
	if (!srcVar)
		goto $error;

	if (funcName == "Array.Get")
	{
		index = symbolTable.Get<int>(scopeName, "index");
		symbolTable.Set(scopeName, "out", srcVar->GetArrayElement(index)
			, srcVar->GetArrayElementTypeStr());
	}
	else if (funcName == "Array.Set")
	{
		index = symbolTable.Get<int>(scopeName, "index");
		variant_t value;
		if (symbolTable.Get(scopeName, "value", value))
			srcVar->SetArrayElement(index, value);
	}
	else if (funcName == "Array.Push")
	{
		variant_t value;
		if (symbolTable.Get(scopeName, "value", value))
			srcVar->PushArrayElement(value);
	}
	else if (funcName == "Array.Pop")
	{
		variant_t value = srcVar->PopArrayElement();
		symbolTable.Set(scopeName, "out", value, srcVar->GetArrayElementTypeStr());
	}
	else if (funcName == "Array.Size")
	{
		symbolTable.Set<float>(scopeName, "size", (float)srcVar->arSize, "float");
	}
	else if (funcName == "Array.Empty")
	{
		symbolTable.Set<bool>(scopeName, "empty", (srcVar->arSize == 0), "bool");
	}
	else if (funcName == "Array.Clear")
	{
		srcVar->ClearArray();
	}
	else if (funcName == "Array.Clone")
	{
		symbolTable.CopyArray(scopeName, "out", srcVar->var);
	}

	return true;


$error:
	// exception process
	if (funcName == "Array.Get")
	{
		// todo: exception process
	}
	else if (funcName == "Array.Set")
	{
		// nothing
	}
	else if (funcName == "Array.Push")
	{
		// nonthing
	}
	else if (funcName == "Array.Pop")
	{
		// todo: exception process
	}
	else if (funcName == "Array.Size")
	{
		symbolTable.Set<float>(scopeName, "size", 0.f, "float");
	}
	else if (funcName == "Array.Clear")
	{
		// nonthing
	}
	else if (funcName == "Array.Clone")
	{
		// nonthing
	}
	return true;
}


// Map.Get/Set/Has/Size/Clear function process
bool cBasicModule::MapFunction(script::cVirtualMachine &vm
	, const string &scopeName, const string &funcName)
{
	using namespace common::script;
	script::cSymbolTable &symbolTable = vm.m_symbTable;
	script::sVariable *var = symbolTable.FindVarInfo(scopeName, "map");
	if (!var) {
		symbolTable.Set<bool>(scopeName, "out", false, "bool");
		return true; // error, not found map
	}

	string key;
	const int varId = var->var.intVal;
	sVariable *mapVar = nullptr;
	string mapScopeName, mapVarName;
	eSymbolType::Enum valueType; // map value type

	auto it = symbolTable.m_varMap.find(varId);
	if (symbolTable.m_varMap.end() == it)
		goto $error;

	// find source map variable
	mapScopeName = it->second.first;
	mapVarName = it->second.second;
	mapVar = symbolTable.FindVarInfo(mapScopeName, mapVarName);
	if (!mapVar)
		goto $error;

	valueType = mapVar->typeValues[2];

	if (funcName == "Map.Get")
	{
		key = symbolTable.Get<string>(scopeName, "key");
		const variant_t val = mapVar->GetMapValue(symbolTable, key);
		symbolTable.Set(scopeName, "out", val, mapVar->GetMapValueTypeStr());
	}
	else if (funcName == "Map.Set")
	{
		key = symbolTable.Get<string>(scopeName, "key");
		variant_t value;
		if (symbolTable.Get(scopeName, "value", value))
			mapVar->SetMapValue(key, value);
	}
	else if (funcName == "Map.Has")
	{
		symbolTable.Set<bool>(scopeName, "result", mapVar->HasMapValue(key), "bool");
	}
	else if (funcName == "Map.Size")
	{
		symbolTable.Set<float>(scopeName, "size", (float)mapVar->GetMapSize(), "float");
	}
	else if (funcName == "Map.Empty")
	{
		symbolTable.Set<bool>(scopeName, "empty", (mapVar->GetMapSize() == 0), "bool");
	}
	else if (funcName == "Map.Clear")
	{
		mapVar->ClearMap();
	}
	else if (funcName == "Map.Clone")
	{
		symbolTable.CopyMap(scopeName, "out", mapVar->var);
	}

	return true;


$error:
	// exception process
	if (funcName == "Map.Get")
	{
		// todo: exception process
	}
	else if (funcName == "Map.Set")
	{
		// nothing
	}
	else if (funcName == "Map.Has")
	{
		symbolTable.Set<bool>(scopeName, "result", false, "bool");
	}
	else if (funcName == "Map.Size")
	{
		symbolTable.Set<float>(scopeName, "size", 0.f, "float");
	}
	else if (funcName == "Map.Clear")
	{
		// nothing
	}
	else if (funcName == "Map.Clone")
	{
		// nothing
	}

	return true;
}


// close module
bool cBasicModule::CloseModule(cVirtualMachine &vm)
{
	return true;
}
