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
			ldcmp,
			getb, geti, getf, gets,
			setb, seti, setf, sets,
			eqi, eqf, eqs,
			eqic, eqfc, eqsc,
			lesi, lesf, leqi, leqf,
			gri, grf, greqi, greqf,
			call, jnz, jmp, label,
			symbolb, symboli, symbolf, symbols,
			cmt, nop
		);

		VARTYPE GetVarType(const eCommand::Enum cmd);


		// intermediate code instruction data
		struct sInstruction {
			eCommand::Enum cmd;
			string str1; // function name, variable name, etc..
			string str2; // function name, variable name, etc..
			uint reg1; // register index (val0, val1, ...)
			uint reg2; // register index (val0, val1, ...)
			variant_t var1; // variable value

			sInstruction() : var1((int)0) {}
			sInstruction(const sInstruction &rhs);
			sInstruction(const eCommand::Enum cmd0) : cmd(cmd0), var1((int)0) {}
			sInstruction(const eCommand::Enum cmd0, const string &_str1) 
				: cmd(cmd0), str1(_str1), var1((int)0) {}
			sInstruction(const eCommand::Enum cmd0, const string &_str1
				, const string &_str2)
				: cmd(cmd0), str1(_str1), str2(_str2), var1((int)0) {}
			~sInstruction();

			sInstruction& operator=(const sInstruction &rhs);
		};

		//
		// - intermediate code instruction syntax - 
		//
		// ldbc register_name, value ;load constant bool type value
		// ldic register_name, value ;load constant int type value
		// ldfc register_name, value ;load constant float type value
		// ldsc register_name, value ;load constant string type value
		// ldcmp register_name ;load compare flag, bool type value
		// getb scope_name, varname, register_name ;load bool type value from symboltable
		// geti scope_name, varname, register_name ;load int type value from symboltable
		// getf scope_name, varname, register_name ;load float type value from symboltable
		// gets scope_name, varname, register_name ;load string type value from symboltable
		// setb scope_name, varname, register_name ;store bool type value to symboltable
		// seti scope_name, varname, register_name ;store int type value to symboltable
		// setf scope_name, varname, register_name ;store float type value to symboltable
		// sets scope_name, varname, register_name ;store string type value to symboltable
		// eqi register_name1, register_name2 ;compare 2 register int type, if equal, cmp register true
		// eqf register_name1, register_name2 ;compare 2 register float type, if equal, cmp register true
		// eqs register_name1, register_name2 ;compare 2 register string type, if equal, cmp register true
		// eqic register_name, value ;compare register, constant int type value, if equal, cmp register true
		// eqfc register_name, value ;compare register, constant float type value, if equal, cmp register true
		// eqsc register_name, value ;compare register, constant string type value, if equal, cmp register true
		// lesi register_name1, register_name2 ;int type compare, if register_name1 < register_name2, then cmp register true
		// lesf register_name1, register_name2 ;float type compare, if register_name1 < register_name2, then cmp register true
		// leqi register_name1, register_name2 ;int type compare, if register_name1 <= register_name2, then cmp register true
		// leqf register_name1, register_name2 ;float type compare, if register_name1 <= register_name2, then cmp register true
		// gri register_name1, register_name2 ;int type compare, if register_name1 > register_name2, then cmp register true
		// grf register_name1, register_name2 ;float type compare, if register_name1 > register_name2, then cmp register true
		// greqi register_name1, register_name2 ;int type compare, if register_name1 >= register_name2, then cmp register true
		// greqf register_name1, register_name2 ;float type compare, if register_name1 >= register_name2, then cmp register true
		// call function_name ;call callback function, and then customizing function work
		// jnz jump_label ;jump jump_label if cmp register is true
		// jmp jump_label ;jump jump_label always
		// label label_name ;jump label name, nothing to operate
		// symbolb scope_name, varname, value ;initialize bool type symbol table
		// symboli scope_name, varname, value ;initialize int type symbol table
		// symbolf scope_name, varname, value ;initialize float type symbol table
		// symbols scope_name, varname, value ;initialize string type symbol table
		// #comment command string, from pinId, to pinId ;comment string, no instruction
		// nop ;nothing to operate


		// interpreter function callback interface
		class cSymbolTable;
		interface iFunctionCallback
		{
			virtual int Function(cSymbolTable &symbolTable
				, const string &scopeName
				, const string &funcName
				, void *arg) = 0;
		};

	}
}
