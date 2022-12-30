//
// 2021-10-20, jjuiddong
// basic script function module
//	- basic function, toString, toNumber ...etc
//	- array, map function
//
#pragma once


namespace common
{

	// script function handling function
	typedef script::eModuleResult (*ModuleFunc) (
		script::cVirtualMachine& vm
		, const string& scopeName
		, const string& funcName
		, void *arg
		);


	class cBasicModule : public script::iModule
	{
	public:
		cBasicModule();
		virtual ~cBasicModule();

		virtual script::eModuleResult Execute(script::cVirtualMachine &vm
			, const string &scopeName, const string &funcName) override;

		virtual bool CloseModule(script::cVirtualMachine &vm) override;


	public:
		map<string, ModuleFunc> m_fnMap;
	};
}
