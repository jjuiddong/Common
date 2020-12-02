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
			ldcmp, ldncmp, ldtim,
			getb, geti, getf, gets,
			setb, seti, setf, sets,
			addi, addf,
			subi, subf,
			muli, mulf,
			divi, divf,
			eqi, eqf, eqs,
			eqic, eqfc, eqsc,
			lesi, lesf, leqi, leqf,
			gri, grf, greqi, greqf,
			call, jnz, jmp, label,
			symbolb, symboli, symbolf, symbols,
			timer1, timer2,
			cmt, delay, nop
		);

		VARTYPE GetVarType(const eCommand::Enum cmd);


		// intermediate code instruction data
		struct sInstruction {
			eCommand::Enum cmd;
			string str1; // function name, variable name, etc..
			string str2; // function name, variable name, etc..
			uint reg1; // register index (val0, val1, ...), or variable id
			uint reg2; // register index (val0, val1, ...), or variable id
			variant_t var1; // variable value

			sInstruction() : reg1(0), reg2(0) {}
			sInstruction(const sInstruction &rhs);
			sInstruction(const eCommand::Enum cmd0) : cmd(cmd0), reg1(0), reg2(0) {}
			sInstruction(const eCommand::Enum cmd0, const string &_str1) 
				: cmd(cmd0), str1(_str1), reg1(0), reg2(0) {}
			sInstruction(const eCommand::Enum cmd0, const string &_str1
				, const string &_str2)
				: cmd(cmd0), str1(_str1), str2(_str2), reg1(0), reg2(0) {}
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
		// ldncmp register_name ;load negate compare flag, bool type value
		// ldtim register_name ;load time flag from register, float type value
		// getb scope_name, varname, register_name ;load register bool type value from symboltable
		// geti scope_name, varname, register_name ;load register int type value from symboltable
		// getf scope_name, varname, register_name ;load register float type value from symboltable
		// gets scope_name, varname, register_name ;load register string type value from symboltable
		// setb scope_name, varname, register_name ;store bool type value to symboltable
		// seti scope_name, varname, register_name ;store int type value to symboltable
		// setf scope_name, varname, register_name ;store float type value to symboltable
		// sets scope_name, varname, register_name ;store string type value to symboltable
		// addi register_name1, register_name2 ;add 2 register int type, store register9
		// addf register_name1, register_name2 ;add 2 register float type, store register9
		// subi register_name1, register_name2 ;subtract 2 register int type, store register9
		// subf register_name1, register_name2 ;subtract 2 register float type, store register9
		// muli register_name1, register_name2 ;multiply 2 register int type, store register9
		// mulf register_name1, register_name2 ;multiply 2 register float type, store register9
		// divi register_name1, register_name2 ;multiply 2 register int type, store register9
		// divf register_name1, register_name2 ;multiply 2 register float type, store register9
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
		// timer1 scope_name, value ; loop timer, value:interval (milliseconds)
		// timer2 scope_name, value ; timer, value:interval (milliseconds), loop on/off
		// #comment command string, from pinId, to pinId ;comment string, no instruction
		// delay ; time flag -= dt, execute next instruction until time flag 0
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


		// synchronize symbol variable (to marshalling)
		struct sSyncSymbol
		{
			const string *scope;
			const string *name;
			const variant_t *var;

			sSyncSymbol(const string *scope_ = nullptr, const string *name_ = nullptr
				, const variant_t *var_ = nullptr) 
			: scope(scope_), name(name_), var(var_) { }
		};

	}
}

