//
// Mixing 명령어 스크립트 파싱
//
#pragma once

#include "mathscript.h"

namespace mathscript
{
	class cMathParser
	{
	public:
		cMathParser();
		virtual ~cMathParser();

		bool Read(const string &fileName);
		bool ParseStr(const string &source);

		mathscript::sStatement *m_stmt;


	protected:
		// BNF
		//- statement -> { assign_stmt  | expression }
		//- assign_stmt -> id = expression
		//- expression -> term +- expression | term
		//- term -> dterm * term | dterm
		//- dterm -> factor / factor | factor
		//- factor -> id, number, func, (expression)
		//- func -> id( expr_list )
		//- expr_list -> expression {,expression}
		// 
		mathscript::sStatement* statement(string &src);
		mathscript::sAssignStmt* assign_stmt(string &src);
		mathscript::sExpr* expression(string &src);
		mathscript::sTerm* term(string &src);
		mathscript::sDTerm* dterm(string &src);
		mathscript::sFactor* factor(string &src);
		mathscript::sFunc* func(const string &name, string &src);
		mathscript::sExprList* expression_list(string &src, const bool isFirst);
	};

}
