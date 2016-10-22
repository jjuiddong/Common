//
// Mixing math command interpreter
//
#pragma once

#include "scriptutil.h"
#include "mathscript.h"


namespace mathscript
{

	class cMathInterpreter
	{
	public:
		cMathInterpreter();
		virtual ~cMathInterpreter();

		float Excute(const sStatement *p);


	protected:
		float assignstmt(const sAssignStmt *p);
		float expr(const sExpr*p);
		float term(const sTerm *p);
		float factor(const sFactor *p);
		float func(const sFunc *p);
	};

}
