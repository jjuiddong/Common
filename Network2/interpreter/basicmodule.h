//
// 2021-10-20, jjuiddong
// basic script function module
//	- basic function, toString, toNumber ...etc
//	- array, map function
//
#pragma once


namespace network2
{
	class cBasicModule : public common::script::iModule
	{
	public:
		cBasicModule();
		virtual ~cBasicModule();

		virtual script::eModuleResult Execute(script::cVirtualMachine &vm
			, const string &scopeName, const string &funcName) override;

		virtual bool CloseModule(script::cVirtualMachine &vm) override;


	protected:
		bool ArrayFunction(script::cVirtualMachine &vm
			, const string &scopeName, const string &funcName);
		bool MapFunction(script::cVirtualMachine &vm
			, const string &scopeName, const string &funcName);
	};
}
