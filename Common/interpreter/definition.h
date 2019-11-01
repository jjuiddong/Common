//
// 2019-10-29, jjuiddong
// interpreter global definition
//
#pragma once


namespace common
{
	namespace script
	{

		// intermediate code command
		DECLARE_ENUM( eCommand,
			none,
			ldbc, ldic, ldfc, ldsc,
			getb, geti, getf, gets,
			setb, seti, setf, sets,
			eqi, eqf, eqs,
			eqic, eqfc, eqsc,
			lesi, lesf, leqi, leqf,
			gri, grf, greqi, greqf,
			call, jnz, jmp, label, nop
		);

		VARTYPE GetVarType(const eCommand::Enum cmd);


		// intermediate code line command set
		struct sCommandSet {
			eCommand::Enum cmd;
			string str1; // function name, variable name, etc..
			string str2; // function name, variable name, etc..
			uint reg1; // register index (val0, val1, ...)
			uint reg2; // register index (val0, val1, ...)
			variant_t var1; // variable value
		};


		// interpreter function callback interface
		class cSymbolTable;
		interface iFunctionCallback
		{
			virtual int Function(cSymbolTable &symbolTable
				, const StrId &scopeName
				, const StrId &funcName
				, void *arg) = 0;
		};

	}
}

