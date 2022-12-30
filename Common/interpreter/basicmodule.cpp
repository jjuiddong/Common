
#include "stdafx.h"
#include "basicmodule.h"

using namespace common;
using namespace common::script;


// script function handler
eModuleResult ToString(cVirtualMachine& vm, const string& scopeName, const string& funcName, void *arg);
eModuleResult ToNumber(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult ToBoolean(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult CastToBoolean(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult CastToNumber(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult CastToString(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult Floor(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult Ceil(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult Round(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult SetTimeOut(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult ClearTimeOut(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult SetInterval(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult ClearInterval(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult StopTick(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult ArrayFunction(script::cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult MapFunction(script::cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);
eModuleResult FnVector3(script::cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg);


cBasicModule::cBasicModule()
{
	m_fnMap.insert({ "ToString", ToString });
	m_fnMap.insert({ "ToNumber", ToNumber});
	m_fnMap.insert({ "ToBoolean", ToBoolean});
	m_fnMap.insert({ "CastToBoolean", CastToBoolean});
	m_fnMap.insert({ "CastToNumber", CastToNumber});
	m_fnMap.insert({ "CastToString", CastToString});
	m_fnMap.insert({ "Floor", Floor});
	m_fnMap.insert({ "Ceil", Ceil});
	m_fnMap.insert({ "Round", Round});
	m_fnMap.insert({ "SetTimeOut", SetTimeOut});
	m_fnMap.insert({ "SetTimer", SetTimeOut });
	m_fnMap.insert({ "ClearTimeOut", ClearTimeOut });
	m_fnMap.insert({ "StopTimer", ClearTimeOut });
	m_fnMap.insert({ "SetInterval", SetInterval});
	m_fnMap.insert({ "ClearInterval", ClearInterval });
	m_fnMap.insert({ "StopTick", StopTick});
	m_fnMap.insert({ "Stop Tick", StopTick });

	m_fnMap.insert({ "Array.Get", ArrayFunction });
	m_fnMap.insert({ "Array.Set", ArrayFunction });
	m_fnMap.insert({ "Array.Push", ArrayFunction });
	m_fnMap.insert({ "Array.Pop", ArrayFunction });
	m_fnMap.insert({ "Array.Find", ArrayFunction });
	m_fnMap.insert({ "Array.Size", ArrayFunction });
	m_fnMap.insert({ "Array.Empty", ArrayFunction });
	m_fnMap.insert({ "Array.Clear", ArrayFunction });
	m_fnMap.insert({ "Array.Clone", ArrayFunction });

	m_fnMap.insert({ "Map.Get", MapFunction });
	m_fnMap.insert({ "Map.Set", MapFunction });
	m_fnMap.insert({ "Map.Has", MapFunction });
	m_fnMap.insert({ "Map.Size", MapFunction });
	m_fnMap.insert({ "Map.Empty", MapFunction });
	m_fnMap.insert({ "Map.Clear", MapFunction });
	m_fnMap.insert({ "Map.Clone", MapFunction });

	m_fnMap.insert({ "Vector3", FnVector3 });

}

cBasicModule::~cBasicModule()
{
}


// execute script function
eModuleResult cBasicModule::Execute(cVirtualMachine &vm
	, const string &scopeName, const string &funcName)
{
	auto it = m_fnMap.find(funcName);
	if (m_fnMap.end() != it)
		return it->second(vm, scopeName, funcName, this);
	return eModuleResult::NoHandler;
}


// close module
bool cBasicModule::CloseModule(cVirtualMachine &vm)
{
	return true;
}



//-----------------------------------------------------------------------------
// script function handler

// ToString
eModuleResult ToString(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	script::sVariable* var = symbolTable.FindVarInfo(scopeName, "in");
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


eModuleResult ToNumber(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	script::sVariable* var = symbolTable.FindVarInfo(scopeName, "string");
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


eModuleResult ToBoolean(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	script::sVariable* var = symbolTable.FindVarInfo(scopeName, "string");
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


eModuleResult CastToBoolean(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	script::sVariable* var = symbolTable.FindVarInfo(scopeName, "in");
	if (var)
		symbolTable.Set<bool>(scopeName, "out", (bool)var->var, "bool");
	else
		symbolTable.Set<bool>(scopeName, "out", false, "bool"); // exception process
	return eModuleResult::Done;
}


eModuleResult CastToNumber(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	script::sVariable* var = symbolTable.FindVarInfo(scopeName, "in");
	if (var)
		symbolTable.Set<float>(scopeName, "out", (float)var->var, "float");
	else
		symbolTable.Set<float>(scopeName, "out", 0.f, "float"); // exception process
	return eModuleResult::Done;
}


eModuleResult CastToString(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	script::sVariable* var = symbolTable.FindVarInfo(scopeName, "in");
	if (var)
	{
		const string str = common::variant2str(var->var);
		symbolTable.Set<string>(scopeName, "out", str, "string");
	}
	else
		symbolTable.Set<string>(scopeName, "out", "null", "string"); // exception process
	return eModuleResult::Done;
}


eModuleResult Floor(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	const float value = symbolTable.Get<float>(scopeName, "number");
	symbolTable.Set<float>(scopeName, "out", floor(value), "float");
	return eModuleResult::Done;
}


eModuleResult Ceil(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	const float value = symbolTable.Get<float>(scopeName, "number");
	symbolTable.Set<float>(scopeName, "out", ceil(value), "float");
	return eModuleResult::Done;
}


eModuleResult Round(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	const float value = symbolTable.Get<float>(scopeName, "number");
	symbolTable.Set<float>(scopeName, "out", round(value), "float");
	return eModuleResult::Done;
}


eModuleResult SetTimeOut(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	const int id = symbolTable.Get<int>(scopeName, "id");
	const int time = symbolTable.Get<int>(scopeName, "time ms"); // milliseconds
	const bool result = vm.SetTimer(id, time);
	symbolTable.Set<bool>(scopeName, "result", result, "bool");
	return eModuleResult::Done;
}


eModuleResult ClearTimeOut(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	const int id = symbolTable.Get<int>(scopeName, "id");
	const bool result = vm.StopTimer(id);
	symbolTable.Set<bool>(scopeName, "result", result, "bool");
	return eModuleResult::Done;
}


eModuleResult SetInterval(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	const int id = symbolTable.Get<int>(scopeName, "id");
	const int time = symbolTable.Get<int>(scopeName, "interval ms"); // milliseconds
	const bool result = vm.SetTimer(id, time, true);
	symbolTable.Set<bool>(scopeName, "result", result, "bool");
	return eModuleResult::Done;
}


eModuleResult ClearInterval(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	const int id = symbolTable.Get<int>(scopeName, "id");
	const bool result = vm.StopTimer(id);
	symbolTable.Set<bool>(scopeName, "result", result, "bool");
	return eModuleResult::Done;
}


eModuleResult StopTick(cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	const int id = symbolTable.Get<int>(scopeName, "id");
	const bool result = vm.StopTick(id);
	symbolTable.Set<bool>(scopeName, "result", result, "bool");
	return eModuleResult::Done;
}


eModuleResult FnVector3(script::cVirtualMachine& vm, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	const float x = symbolTable.Get<float>(scopeName, "x");
	const float y = symbolTable.Get<float>(scopeName, "y");
	const float z = symbolTable.Get<float>(scopeName, "z");
	const vector<float> ar = { x, y, z };
	symbolTable.Set(scopeName, "out", ar, "Array<Float>");

	return eModuleResult::Done;
}


// Array.Get/Set/Push/Pop/Find/Size function process
eModuleResult ArrayFunction(script::cVirtualMachine& vm
	, const string& scopeName, const string& funcName, void* arg)
{
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	script::sVariable* var = symbolTable.FindVarInfo(scopeName, "array");
	if (!var) {
		symbolTable.Set<bool>(scopeName, "out", false);
		return eModuleResult::Done; // error, not found array
	}

	int index = 0;
	const int varId = var->var.intVal;
	script::sVariable* srcVar = nullptr;

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
	else if (funcName == "Array.Find")
	{
		variant_t value;
		if (symbolTable.Get(scopeName, "value", value))
		{
			const int ret = srcVar->FindArrayElement(value);
			symbolTable.Set(scopeName, "index", (float)ret, "float");
		}
		else
		{
			symbolTable.Set(scopeName, "index", -1.f, "float");
		}
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

	return eModuleResult::Done;


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
	return eModuleResult::Done;
}


// Map.Get/Set/Has/Size/Clear function process
eModuleResult MapFunction(script::cVirtualMachine& vm
	, const string& scopeName, const string& funcName, void* arg)
{
	using namespace common::script;
	script::cSymbolTable& symbolTable = vm.m_symbTable;
	script::sVariable* var = symbolTable.FindVarInfo(scopeName, "map");
	if (!var) {
		symbolTable.Set<bool>(scopeName, "out", false, "bool");
		return eModuleResult::Done; // error, not found map
	}

	string key;
	const int varId = var->var.intVal;
	sVariable* mapVar = nullptr;
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

	return eModuleResult::Done;


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

	return eModuleResult::Done;
}
