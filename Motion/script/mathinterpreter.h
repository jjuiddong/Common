//
// Mixing math command interpreter
//
//
// 2016-10-25
//		- quat2roll(x,y,z,w)
//		- quat2pitch(x,y,z,w)
//		- quat2yaw(x,y,z,w)
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
		// Grammar
		// assignedstmt -> id = expr
		// expr -> term [+|-] expr
		// term -> dterm * term
		// dterm -> factor / factor | factor
		// factor -> id | number | (expr) | function

		float assignstmt(const sAssignStmt *p);
		float expr(const sExpr*p);
		float term(const sTerm *p);
		float dterm(const sDTerm *p);
		float factor(const sFactor *p);
		float func(const sFunc *p);
	};

}
