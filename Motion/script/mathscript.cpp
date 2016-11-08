
#include "stdafx.h"
#include "mathscript.h"

using namespace mathscript;


void mathscript::rm_statement(sStatement *p)
{
	RET(!p);
	rm_assignstmt(p->assign);
	rm_expr(p->expr);
	rm_statement(p->next);
	delete p;
}

void mathscript::rm_assignstmt(sAssignStmt *p)
{
	RET(!p);
	rm_expr(p->expr);
	delete p;
}

void mathscript::rm_factor(sFactor *p)
{
	RET(!p);
	rm_expr(p->expr);
	rm_func(p->func);
	delete p;
}

void mathscript::rm_term(sTerm *p)
{
	RET(!p);
	rm_dterm(p->dterm);
	rm_term(p->term);
	delete p;
}

void mathscript::rm_dterm(sDTerm *p)
{
	RET(!p);
	rm_factor(p->factor1);
	rm_factor(p->factor2);
	delete p;
}


void mathscript::rm_expr(sExpr *p)
{
	RET(!p);
	rm_term(p->term1);
	rm_expr(p->expr);
	delete p;
}

void mathscript::rm_func(sFunc *p)
{
	RET(!p);
	rm_exprlist(p->exprs);
	delete p;
}

void mathscript::rm_exprlist(sExprList *p)
{
	RET(!p);
	rm_exprlist(p->next);
	delete p;
}
