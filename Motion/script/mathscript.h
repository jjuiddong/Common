// 믹싱 스크립트 구조체 선언

#pragma once

namespace mathscript 
{

	//-----------------------------------------------------------------------------------------
	// Mixing Command Parser 
	struct sFactor;

	namespace OP_MULTI {
		enum TYPE {
			MULTI, DIVIDE
		};
	}

	namespace OP_PLUS {
		enum TYPE {
			PLUS, MINUS
		};
	}

	namespace FACTOR_TYPE {
		enum  TYPE
		{
			ID,
			NUMBER,
			EXPR,
			FUNC,
		};
	}

	struct sTerm
	{
		sFactor *factor1;
		OP_MULTI::TYPE op;
		//sFactor *factor2;
		sTerm *term;
	};

	struct sExpr
	{
		sTerm *term1;
		OP_PLUS::TYPE op;
		sExpr *expr;
	};

	struct sExprList
	{
		sExpr *expr;
		sExprList *next;
	};

	struct sFunc
	{
		string id;
		//sExpr *expr;
		sExprList *exprs;
	};

	struct sFactor
	{
		FACTOR_TYPE::TYPE type;
		string id;
		float number;
		sExpr *expr;
		sFunc *func;
	};

	struct sAssignStmt
	{
		string id;
		sExpr *expr;
	};

	struct sStatement
	{
		sAssignStmt *assign;
		sExpr *expr;
		sStatement *next;
	};


	void rm_statement(sStatement *p);
	void rm_assignstmt(sAssignStmt *p);
	void rm_factor(sFactor *p);
	void rm_term(sTerm *p);
	void rm_expr(sExpr *p);
	void rm_func(sFunc*p);
	void rm_exprlist(sExprList *p);
}

